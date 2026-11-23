#ifndef MANHATTANMAP_UTIL_H
#define MANHATTANMAP_UTIL_H

#include <stdint.h>

#include "quickpix.h"

typedef struct Color_t {
    uint8_t x, r, g, b; // ordered for hardware + padding reasons
} Color;

// group of multiple pixel chains where each chain is on its own data line
typedef struct PixelGroup_t {
    float (*positions)[2]; // array of float[2] (x, y)
    Color* colors;
    PixelIndex count;
} PixelGroup;

typedef void (*PixelTransformFn)(const PixelGroup*, PixelIndex, void*);

// COLOR

Color color_from(uint8_t r, uint8_t g, uint8_t b);
Color color_mix(Color c1, Color c2, float mix); // mix [0, 1) = % contribution of c2 (remaining is c1)

// PIXEL GROUP

void pxg_init(PixelGroup* pxg, const Quickpix* quickpix);
void pxg_apply(const PixelGroup* pxg, PixelTransformFn fn, void* aux); // apply function to each pixel in group

// PIXEL DATA

Color px_get_color(const PixelGroup* pxg, PixelIndex px);
void px_set_color(const PixelGroup* pxg, PixelIndex px, Color color);
void px_get_position(const PixelGroup* pxg, PixelIndex px, float* out_x, float* out_y);
void px_set_position(const PixelGroup* pxg, PixelIndex px, float x, float y);

#endif //MANHATTANMAP_UTIL_H