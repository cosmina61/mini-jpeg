#include "color.hpp"

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <random>
#include <algorithm>

using namespace mjpeg;

static int failures = 0;

static void check(bool ok, const char* what)
{
    if(!ok)
    {
        std::cerr << "FAILED: " << what << std::endl;
        ++failures;
    }
}

int main()
{
    YCbCrPixel white = rgb_to_ycbcr({255, 255, 255});
    check(std::fabs(white.y - 255.0f) < 0.01f, "white Y is 255");
    check(std::fabs(white.cb - 128.0f) < 0.01f, "white Cb is 128");
    check(std::fabs(white.cr - 128.0f) < 0.01f, "white Cr is 128");

    YCbCrPixel black = rgb_to_ycbcr({0, 0, 0});

    check(std::fabs(black.y - 0.0f) < 0.01f, "black Y is 0");

    std::mt19937 rng(12345);  
    std::uniform_int_distribution<int> dist(0, 255);
    int worst=0;

    for (int i = 0; i < 100000; ++i) {
                RgbPixel in{static_cast<std::uint8_t>(dist(rng)),
                            static_cast<std::uint8_t>(dist(rng)),
                            static_cast<std::uint8_t>(dist(rng))};
                RgbPixel out = ycbcr_to_rgb(rgb_to_ycbcr(in));
                worst = std::max({worst,
                                std::abs(int(in.r) - int(out.r)),
                                std::abs(int(in.g) - int(out.g)),
                                std::abs(int(in.b) - int(out.b))});
    }

    check(worst <= 1, "round trip error is at most 1 level");
    std::cout << "worst round-trip error: " << worst << "\n";

    if (failures > 0) {
        std::cerr << failures << " check(s) failed\n";
        return EXIT_FAILURE;
    }
    std::cout << "all color tests passed\n";
    return EXIT_SUCCESS;
}