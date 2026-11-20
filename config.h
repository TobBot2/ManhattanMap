#ifndef MANHATTANMAP_CONFIG_H
#define MANHATTANMAP_CONFIG_H

#include "Pixel.h"
#include "TrainLine.h"

typedef struct ChainSegmentConfig_t {
    PixelIndex begin;
    float begin_x, begin_y;
    PixelIndex end; // inclusive
    float end_x, end_y;
} ChainSegmentConfig;

typedef struct ChainConfig_t {
    int pin;
    PixelIndex leds_count;
    const ChainSegmentConfig* pixel_segments_positions;
    unsigned pixel_segment_positions_count;
} ChainConfig;

typedef struct TrainLineSegmentConfig_t {
    PixelHandle begin;
    PixelHandle end; // inclusive
} TrainLineSegmentConfig;

typedef struct TrainLineConfig_t {
    TrainLineName train_line;
    const TrainLineSegmentConfig* segment;
    unsigned segments_count;
} TrainLineConfig;

typedef struct Config_t {
    struct {
        int pin;
        int debounce_ms;
    } button;

    const ChainConfig* chain_configs;
    unsigned chains_count;

    const TrainLineConfig* tl_configs;
    unsigned tls_count;

    float max_brightness;
    float target_fps;
} Config;

extern const Config CONFIG;

#endif //MANHATTANMAP_CONFIG_H