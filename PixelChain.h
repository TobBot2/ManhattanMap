#ifndef MANHATTANMAP_UTIL_H
#define MANHATTANMAP_UTIL_H

#include <stdint.h>
#include <stdlib.h>

#include "hardware/pio.h"
#include "pico/types.h"

typedef uint16_t PixelIndex; // limit is small due to 5m limit of lights anyway

typedef struct Color_t {
    uint8_t g, r, b; // stored grb order for hardware reasons
} Color;

typedef struct PIOHandle_t {
    PIO pio;
    uint sm;
    uint offset;
} PIOHandle;

typedef struct PixelData_t {
    float x, y;
    Color color;
} PixelData;

typedef struct PixelChain_t {
    uint32_t* pixels;
    PixelData* data;
    PixelIndex count;
} PixelChain;

typedef struct PixelHandle_t {
    PixelIndex index;
    PixelIndex chain;
} PixelHandle;

typedef struct PixelGroup_t {
    PixelChain* chains;
    unsigned count;
} PixelGroup;

uint32_t color_to_hardware_format(Color c);

PixelData* px_get_data(PixelHandle px, const PixelGroup* pixel_group);

void px_set_intersection(PixelHandle px, const PixelGroup* pixel_group, const PixelHandle* neighbors, unsigned neighbors_cnt);
PixelHandle* px_get_neighbors(PixelHandle px, const PixelGroup* pixel_group);

void px_set_color(PixelHandle px, const PixelGroup* pixel_group, Color color);
Color px_get_color(PixelHandle px, const PixelGroup* pixel_group);

void px_set_position(PixelHandle px, const PixelGroup* pixel_group, float x, float y);
void px_set_strip_positions(PixelHandle px, const PixelGroup* pixel_group, unsigned count, float x1, float y1, float x2, float y2);
void px_get_position(PixelHandle px, const PixelGroup* pixel_group, float* x, float* y);

#endif //MANHATTANMAP_UTIL_H