#include "brightness.h"
#include <cstring>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <xsimd/xsimd.hpp>

extern "C" {

#include <xsimd/xsimd.hpp>
#include <cstdint>
#include <algorithm>

void increase_brightness(uint8_t* data, int width, int height, int stride, uint8_t delta)
{
    using batch = xsimd::batch<uint8_t>;
    constexpr std::size_t simd_size = batch::size;

    batch vdelta(delta);
    batch vmax(255);

    for (int y = 0; y < height; ++y)
    {
        uint8_t* row = data + y * stride;
        int totalBytes = width * 4;
        int x = 0;

        // SIMD loop
        for (; x + simd_size <= totalBytes; x += simd_size)
        {
            batch pixels = batch::load_unaligned(row + x);

            // Temporary array to manipulate only RGB
            alignas(16) uint8_t tmp[simd_size];
            pixels.store_unaligned(tmp);

            for (std::size_t i = 0; i < simd_size; ++i)
            {
                if (i % 4 != 3) // skip alpha
                    tmp[i] = std::min<int>(tmp[i] + delta, 255);
            }

            batch::load_unaligned(tmp).store_unaligned(row + x);
        }

        // Scalar tail
        for (; x < totalBytes; ++x)
        {
            if (x % 4 != 3)
                row[x] = std::min<int>(row[x] + delta, 255);
        }
    }
}
}