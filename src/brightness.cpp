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

    batch vdelta(delta);   // SIMD vector with delta
    batch vmax(255);       // SIMD vector with 255 for clamping

    for (int y = 0; y < height; ++y)
    {
        uint8_t* row = data + y * stride;
        int totalBytes = width * 4;
        int x = 0;

        // SIMD loop
        for (; x + simd_size <= totalBytes; x += simd_size)
        {
            batch pixels = batch::load_unaligned(row + x);  //loads pixels into SIMD register

            // Create mask for RGB channels (skip alpha)
            alignas(simd_size) uint8_t mask_arr[simd_size];
            for (std::size_t i = 0; i < simd_size; ++i)
                mask_arr[i] = (i % 4 != 3) ? 0xFF : 0x00;

            batch mask = batch::load_aligned(mask_arr);

            // Add delta only to RGB channels and clamp
            batch result = xsimd::min(pixels + (vdelta & mask), vmax);

            result.store_unaligned(row + x);   // back into memory
        }
        // Tail loop for remaining pixels
        for (; x < totalBytes; ++x)
        {
            if (x % 4 != 3)
                row[x] = std::min<int>(row[x] + delta, 255);
        }
    }
}
}
// void increase_brightness(uint8_t* data, int width, int height, int stride, uint8_t delta)
// {
//     for (int y = 0; y < height; ++y)
//     {
//         uint8_t* row = data + y * stride;

//         for (int x = 0; x < width; ++x)
//         {
//             int idx = x * 4;

//             // Add delta to R, G, B channels only, clamp at 255
//             for (int c = 0; c < 3; ++c) // 0=R, 1=G, 2=B
//             {
//                 int tmp = row[idx + c] + delta;
//                 row[idx + c] = (tmp > 255) ? 255 : tmp;
//             }

//             // Alpha channel (row[idx + 3]) remains unchanged
//         }
//     }
// }
// }
/*
sse2 16 byte
avx2 32 byte
avx512 64 byte
neon (arm) 16byte elements
vector bits bytes*8.  
*/