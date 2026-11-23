#pragma once  // stops the file from being included twice

#include <cstdint>

namespace mjpeg{

//one pixel of 8 bits rgb
struct RgbPixel
{
    std::uint8_t r,g,b;
};

struct YCbCrPixel
{
    float y,cb,cr;
};

YCbCrPixel rgb_to_ycbcr(RgbPixel p);
RgbPixel ycbcr_to_rgb(YCbCrPixel p);

}