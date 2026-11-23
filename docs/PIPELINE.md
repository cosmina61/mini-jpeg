# mini-jpeg pipeline

## Scope

- Format: baseling sequentioal JPEG (ITU-T T.81), 8-bit samples
- Entropy coding: Huffman with the standard Annex K tables
- Chroma: 4:2:0 subsampling for color images
- Order of work: grayscale first, then color
- Out of scope: progressive mode, arithmetic coding, 12-bit samples, lossless JPEG, EXIF and other metadata, and restart markers

## Stages

### 1. Color conversion (RGB to YCbCr)
- Input: 8-bit RGB pixels, interleaved (R, G, B per pixel)
- Output: three float planes, Y, Cb, Cr, each at full resolution
- Formula (BT.601, full range, as used by JFIF):
  - Y  = 0.299 R + 0.587 G + 0.114 B
  - Cb = -0.168736 R - 0.331264 G + 0.5 B + 128
  - Cr = 0.5 R - 0.418688 G - 0.081312 B + 128
- Inverse: R = Y + 1.402 (Cr - 128), G = Y - 0.344136 (Cb - 128) - 0.714136 (Cr - 128), B = Y + 1.772 (Cb - 128)
- Why: the eye is more sensitive to brightness than color, so separating them lets chroma be reduced without a visible cost.
- Runs on: CPU


### 2. Chroma subsampling (4:2:0)
- Input: Cb and Cr planes at full resolution
- Output: Cb and Cr planes at half width and half height
- Downsampling: average each 2×2 group of pixels (decide and note whether you use a simple average or a filter)
- Upsampling (decoder): nearest-neighbor or bilinear; record which one you chose
- Edge case: for odd width or height, the last chroma sample covers a partial 2×2 group, so pad the plane before averaging
- Runs on: CPU


### 3. Block splitting and padding
- Input: each plane (Y at full size, Cb and Cr at half size)
- Output: a list of 8×8 blocks per plane, each with 64 values
- Padding rule: extend the plane to a multiple of 8 (for chroma, the multiple is 8 after subsampling, so luma needs a multiple of 16 for complete MCUs) by repeating the last row or column
- Block order: left to right, top to bottom, with each MCU (16×16 luma region plus its chroma blocks) in raster order
- Runs on: CPU

### 4. Level shift and DCT (8×8)
- Input: 64 samples per block, each 0 to 255
- Level shift: subtract 128 from each sample so values center on zero
- Output: 64 DCT coefficients per block, DC at position (0,0)
- Formula: the 2D DCT-II, F(u,v) = (1/4) C(u) C(v) sum over x,y of f(x,y) cos((2x+1)uπ/16) cos((2y+1)vπ/16), where C(0) = 1/√2 and C(k) = 1 otherwise
- Algorithm: naive version first, then the separable version (a 1D DCT applied to rows, then columns)
- Runs on: CPU reference first, then GPU (one kernel, one thread per coefficient at first)

### 5. Quantization
- Input: 64 DCT coefficients per block
- Output: 64 integers per block
- Tables: standard luminance and chrominance tables from Annex K
- Quality scaling (libjpeg mapping): scale = 5000 / quality if quality < 50, else 200 − 2 × quality; each table entry becomes clamp((entry × scale + 50) / 100, 1, 255)
- Rounding: divide and round to the nearest integer
- Runs on: CPU reference first, then GPU

### 6. Zigzag scan
- Input: 64 quantized values in 8×8 row-major order
- Output: the same 64 values in zigzag order (low frequency to high)
- Runs on: CPU

### 7. Run-length coding
- DC handling: encode the difference from the previous block's DC value, per component
- AC symbols: (run of zeros, size of next nonzero value) as a single byte, with ZRL for runs of 16 zeros and EOB when the rest of the block is zero
- Value encoding: magnitude category plus the bits of the value, with negative values encoded in one's complement form
- Runs on: CPU

### 8. Huffman coding
- Tables used: the standard DC and AC luminance and chrominance tables from Annex K, written into DHT segments
- Bit packing: most significant bit first
- Byte stuffing: after every 0xFF byte in the entropy-coded data, insert 0x00
- Padding at the end of the scan: fill the last byte with 1 bits
- Runs on: CPU


## Decoder (reverse order)

1. Parse markers (SOI, DQT, SOF0, DHT, SOS, EOI)
2. Huffman decode, undoing byte stuffing
3. Undo run-length coding and DC differences
4. Inverse zigzag
5. Dequantize (multiply by the table entries)
6. Inverse DCT and add 128
7. Upsample chroma
8. Convert YCbCr to RGB and clamp to 0 to 255

The decoded image is not identical to the input. Quantization and subsampling discard information, and rounding adds small errors.

## Data layout

- In memory, each plane is a row-major array of floats (CPU) or floats in device memory (GPU)
- Blocks are stored contiguously, 64 values each, in block order
- Image edges use the padding rule from stage 3

## Testing plan

- Color conversion: round-trip random pixels, error under 1 level
- Subsampling: sizes 1×1, 17×9, 1921×1080
- DCT: naive and separable versions agree; inverse recovers input within floating-point tolerance
- Quantization: higher quality gives lower error
- Huffman and bitstream: byte stuffing for 0xFF, and round trip through the decoder
- Interoperability: decode our files with libjpeg-turbo and vice versa (Day 9)
- Quality: PSNR and bits per pixel (Day 10)
- Test images: a synthetic gradient, a checkerboard, and one or two real photos

## Open questions

- Chroma downsampling filter: simple average, or a filter with more taps?
- Which upsampling method to use in the decoder?
- How to handle the GPU DCT precision compared with the CPU reference (write the answer on Day 14)
- Which real test photos to use, and where to get them with a license that allows redistribution