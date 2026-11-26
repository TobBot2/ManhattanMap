#ifndef MANHATTANMAP_CONFIG_H
#define MANHATTANMAP_CONFIG_H

typedef struct PixelSegmentConfig_t {
    unsigned begin;
    float begin_x, begin_y;
    unsigned end; // inclusive
    float end_x, end_y;
} PixelSegmentConfig;

typedef struct ChainConfig_t {
    int pin;
    unsigned leds_count;
} ChainConfig;

typedef struct TrainLineSegmentConfig_t {
    unsigned begin;
    unsigned end; // inclusive
} TrainLineSegmentConfig;

typedef struct TrainLineConfig_t {
    struct { unsigned r, g, b; } color;
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