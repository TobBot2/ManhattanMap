#include "config.h"
#include "pico/platform/compiler.h"

// ******************************************************************************
//                                CHAIN CONFIG
// ******************************************************************************

static const ChainConfig CHAINS_CONFIG[] = {
    {
        .pin = 6,
        .leds_count = 618,
    },
    {
        .pin = 7,
        .leds_count = 548,
    }
};

// ******************************************************************************
//                              PIXELSEGMENT CONFIG
// ******************************************************************************

static const PixelSegmentConfig PIXEL_SEGMENTS[] = {
    {
        .begin = 0, .begin_x = 0, .begin_y = 0,
        .end = 617 + 548, .end_x = 0, .end_y = 0
    },
    // TODO x however many segments...
};

// ******************************************************************************
//                              TRAINLINE CONFIG
// ******************************************************************************

static const TrainLineSegmentConfig TRAINLINE_SEGMENTS_1[] = {
    {
        .begin = 0,
        .end = 617+548
    },
    // TODO all segments of the line...
};

// TODO all TRAINLINES
static const TrainLineConfig TRAINLINE_CONFIGS[] = {
    { // [0] = 1 line
        .color = {0xEE, 0x35, 0x2E},
        .segments = TRAINLINE_SEGMENTS_1,
        .segments_count = count_of(TRAINLINE_SEGMENTS_1)
    },
    // TODO x20...
};

// line colors:
// switch (line) {
//     case LINE_1:
//     case LINE_2:
//     case LINE_3:
//         return (Color){ .r = 0xEE, .g = 0x35, .b = 0x2E };
//
//     case LINE_A:
//     case LINE_C:
//     case LINE_E:
//         return (Color){ .r = 0x00, .g = 0x39, .b = 0xA6 };
//
//     case LINE_4:
//     case LINE_5:
//     case LINE_6:
//         return (Color){ .r = 0x00, .g = 0x93, .b = 0x3C };
//
//     case LINE_B:
//     case LINE_D:
//     case LINE_F:
//     case LINE_M:
//         return (Color){ .r = 0xFF, .g = 0x63, .b = 0x19 };
//
//     case LINE_Q:
//     case LINE_N:
//     case LINE_R:
//     case LINE_W:
//         return (Color){ .r = 0xFC, .g = 0xCC, .b = 0x0A };
//
//     case LINE_7:
//         return (Color){ .r = 0xB9, .g = 0x33, .b = 0xAD };
//
//     case LINE_L:
//         return (Color){ .r = 0xA7, .g = 0xA9, .b = 0xAC };
//
//     case LINE_S:
//         return (Color){ .r = 0x80, .g = 0x81, .b = 0x83 };
//
//     default:
//         return (Color){ .r = 0x00, .g = 0x00, .b = 0x00 };
// }

// ******************************************************************************
//                                 MAIN CONFIG
// ******************************************************************************

const Config CONFIG = {
    .button = {
        .pin = 3,
        .debounce_ms = 200
    },

    .chain_configs = CHAINS_CONFIG,
    .chains_count = count_of(CHAINS_CONFIG),

    .pixel_segments = PIXEL_SEGMENTS,
    .pixel_segments_count = count_of(PIXEL_SEGMENTS),

    .trainline_configs = TRAINLINE_CONFIGS,
    .trainlines_count = count_of(TRAINLINE_CONFIGS),

    .max_brightness = .1f,
    .target_fps = 60.f
};

// static_assert(count_of(TRAINLINE_CONFIGS) < 32);