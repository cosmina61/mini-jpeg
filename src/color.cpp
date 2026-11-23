#include "color.hpp"

#include <algorithm>
#include <cmath>

namespace mjpeg{

YCbCrPixel rgb_to_ycbcr(RgbPixel p)
{
    const float r=p.r, g=p.g, b=p.b;
    YCbCrPixel out;

    out.y  =  0.299f    * r + 0.587f    * g + 0.114f    * b;
    out.cb = -0.168736f * r - 0.331264f * g + 0.5f      * b + 128.0f;
    out.cr =  0.5f      * r - 0.418688f * g - 0.081312f * b + 128.0f;

    return out;
}

RgbPixel ycbcr_to_rgb(YCbCrPixel p)
{
   
    const float cb = p.cb - 128.0f;
    const float cr = p.cr - 128.0f;
    const float r = p.y + 1.402f * cr;
    const float g = p.y - 0.344136f * cb - 0.714136f * cr;
    const float b = p.y + 1.772f * cb;

    auto to_byte=[](float v){
        v=std::clamp(std::round(v ),0.0f,255.0f);
        //std::clamp(x, lo, hi) returns x limited to the range lo to hi
        return static_cast<std::uint8_t>(v);
    };
    return RgbPixel{to_byte(r),to_byte(g),to_byte(b)};
}

}