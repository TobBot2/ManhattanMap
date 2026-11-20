#include "config.h"


// ******************************************************************************
//                                CHAIN CONFIG
// ******************************************************************************

#define CHAIN_SEGMENTS_1_COUNT 12
const ChainSegmentConfig CHAIN_SEGMENTS_1[CHAIN_SEGMENTS_1_COUNT] = {
    {
        .begin = 0, .begin_x = 0, .begin_y = 0,
        .end = 0, .end_x = 0, .end_y = 0
    },
    // TODO x however many segments...
};

#define CHAIN_SEGMENTS_2_COUNT 12
const ChainSegmentConfig CHAIN_SEGMENTS_2[CHAIN_SEGMENTS_2_COUNT] = {
    {
        .begin = 0, .begin_x = 0, .begin_y = 0,
        .end = 0, .end_x = 0, .end_y = 0
    },
    // TODO x however many segments...
};

#define CHAINS_COUNT 2
const ChainConfig CHAINS_CONFIG[CHAINS_COUNT] = {
    {
        .pin = 6,
        .leds_count = 618,
        .pixel_segments_positions = CHAIN_SEGMENTS_1,
        .pixel_segment_positions_count = CHAIN_SEGMENTS_1_COUNT
    },
    {
        .pin = 7,
        .leds_count = 548,
        .pixel_segments_positions = CHAIN_SEGMENTS_2,
        .pixel_segment_positions_count = CHAIN_SEGMENTS_2_COUNT
    }
};


// ******************************************************************************
//                              TRAINLINE CONFIG
// ******************************************************************************

#define TRAINLINE_SEGMENTS_1_COUNT 12
const TrainLineSegmentConfig TRAINLINE_SEGMENTS_1[TRAINLINE_SEGMENTS_1_COUNT] = {
    {
        .begin = { .index = 0, .chain = 0, .group = NULL },
        .end = { .index = 0, .chain = 0, .group = NULL }
    },
    // TODO all segments of the line...
};

// TODO all TRAINLINES

#define TRAINLINES_COUNT 20
const TrainLineConfig TRAINLINES_CONFIG[TRAINLINES_COUNT] = {
    {
        .train_line = LINE_1,
        .segment = TRAINLINE_SEGMENTS_1,
        .segments_count = TRAINLINE_SEGMENTS_1_COUNT,
    },
    // TODO x20...
};

// ******************************************************************************
//                                 MAIN CONFIG
// ******************************************************************************

const Config CONFIG = {
    .button = {
        .pin = 3,
        .debounce_ms = 200
    },

    .chain_configs = CHAINS_CONFIG,
    .chains_count = CHAINS_COUNT,

    .tl_configs = TRAINLINES_CONFIG,
    .tls_count = TRAINLINES_COUNT,

    .max_brightness = 0xef,
    .target_fps = 60.f
};