#include <cstdint>


extern "C" {
// Modify RGBA image in-place. stride = bytes per row (usually width*4)
void increase_brightness(uint8_t* data, int width, int height, int stride, uint8_t delta);
}