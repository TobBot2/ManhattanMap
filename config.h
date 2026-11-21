#ifndef MANHATTANMAP_CONFIG_H
#define MANHATTANMAP_CONFIG_H

#include "Pixel.h"

// TODO: should split up dependencies between pixel groups and
//  quickipx, ideally eliminating references to non-primitives

typedef struct PixelSegmentConfig_t {
    PixelHandle begin;
    float begin_x, begin_y;
    PixelHandle end; // inclusive
    float end_x, end_y;
} PixelSegmentConfig;

typedef struct ChainConfig_t {
    int pin;
    PixelIndex leds_count;
} ChainConfig;

typedef struct TrainLineSegmentConfig_t {
    PixelHandle begin;
    PixelHandle end; // inclusive
} TrainLineSegmentConfig;

typedef struct TrainLineConfig_t {
    Color color;
    const TrainLineSegmentConfig* segments;
    unsigned segments_count;
} TrainLineConfig;

typedef struct Config_t {
    struct {
        int pin;
        int debounce_ms;
    } button;

    const ChainConfig* chain_configs;
    unsigned chains_count;

    const PixelSegmentConfig* pixel_segments;
    unsigned pixel_segments_count;

    const TrainLineConfig* trainline_configs;
    unsigned trainlines_count;

    float max_brightness;
    float target_fps;
} Config;

extern const Config CONFIG;

#endif //MANHATTANMAP_CONFIG_H