#include <cstdint> 
#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#else
#define EMSCRIPTEN_KEEPALIVE
#endif

extern "C" {
    EMSCRIPTEN_KEEPALIVE
    void increase_brightness(uint8_t* data, int width, int height, int stride, uint8_t delta);
}