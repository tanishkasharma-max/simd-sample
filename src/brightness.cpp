#include "brightness.h"
#include <cstring>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif



extern "C" {

#include <xsimd/xsimd.hpp>
#include <cstdint>
#include <algorithm>

void increase_brightness(uint8_t* data, int width, int height, int stride, uint8_t delta)
{
    using u8_batch = xsimd::batch<uint8_t>;
    const std::size_t simd_size = u8_batch::size;

    const u8_batch vdelta(delta);
    const u8_batch vmax(255);

    for (int y = 0; y < height; ++y)
    {
        uint8_t* row = data + y * stride;
        int totalBytes = width * 4;
        int x = 0;

        // Process in SIMD chunks
        for (; x + simd_size <= totalBytes; x += simd_size)
        {
            u8_batch pixels = u8_batch::load_unaligned(row + x);

            // Add delta to R, G, B channels only
            alignas(16) uint8_t tmp[simd_size];
            pixels.store_unaligned(tmp);
            for (int i = 0; i < simd_size; ++i)
            {
                if ((i % 4) != 3) // skip alpha
                    tmp[i] = std::min<int>(tmp[i] + delta, 255);
            }

            u8_batch::load_unaligned(tmp).store_unaligned(row + x);
        }

        // Process tail
        for (; x < totalBytes; x++)
        {
            if (x % 4 != 3)
                row[x] = std::min<int>(row[x] + delta, 255);
        }
    }
}
}