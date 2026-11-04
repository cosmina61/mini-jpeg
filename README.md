# mini-jpeg

A JPEG-style encoder and decoder in c++ with an optional CUDA backend for the DCT and quantization stages.

## Status

Just starting. 

## Build
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

To enable the CUDA backend (requires the CUDA toolkit and an NVIDIA GPU):

```bash
cmake -S . -B build -DMJPEG_ENABLE_CUDA=ON
```

## Planned

- RGB to YCbCr color conversion and 4:2:0 subsampling
- 8×8 DCT and quantization (CPU reference and CUDA)
- Zigzag, run-length, and entropy coding
- PSNR versus bitrate and throughput versus image size benchmarks
