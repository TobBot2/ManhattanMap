#ifndef MANHATTANMAP_UTIL_H
#define MANHATTANMAP_UTIL_H

#include <stdint.h>
#include <stdlib.h>

typedef uint16_t PixelIndex; // limit is small due to 5m limit of lights anyway

typedef struct Color_t {
    uint8_t x, g, r, b; // stored grb order for hardware reasons
} Color;

typedef struct PixelData_t {
    float x, y; // in inches, relative to arbitrary point (columbus circle?)
    Color color;
} PixelData;

// chain of pixels (e.g. an LED strip)
typedef struct PixelChain_t {
    PixelData* data;
    PixelIndex count;
} PixelChain;

// group of multiple pixel chains where each chain is on its own data line
typedef struct PixelGroup_t {
    PixelChain* chains;
    unsigned count;
} PixelGroup;

// logical handle describing a particular pixel
typedef struct PixelHandle_t {
    PixelIndex index;
    unsigned chain;
    const PixelGroup* group;
} PixelHandle;

typedef Color (*PixelTransformFn)(PixelHandle, void*);

// COLOR

uint32_t color_to_hardware_format(Color c);

// PIXEL GROUP

void pxg_init(PixelGroup* pxg);
PixelChain* pxg_add_chain(PixelGroup* pxg, unsigned count);
void pxg_apply(const PixelGroup* pxg, PixelTransformFn fn, void* aux); // apply function to each pixel in group

// PIXEL DATA

PixelHandle px_create_empty();
void px_next(PixelHandle* px);
int px_is_empty(PixelHandle px); // boolean return
int px_compare(PixelHandle lhs, PixelHandle rhs); // cmp chain.index output -, 0, + if lhs <, =, > rhs
PixelData* px_get_data(PixelHandle px);
void px_set_color(PixelHandle px, Color color);
Color px_get_color(PixelHandle px);
void px_set_position(PixelHandle px, float x, float y);
void px_set_strip_positions(PixelHandle px, unsigned count, const float p1[2], const float p2[2]); // count is inclusive
void px_get_position(PixelHandle px, float* out_x, float* out_y);

#endif //MANHATTANMAP_UTIL_H