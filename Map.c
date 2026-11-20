#include "Map.h"

#include <stdio.h>

#include "DisplayMode.h"
#include "quickpix.h"
#include "TrainLine.h"

static Quickpix quickpix;

static PixelGroup pixel_group;

static TrainLine* train_lines;
static unsigned train_lines_cnt;

static PixelTrainLineMap pixel_train_line_map;
static IntersectionSet intersection_set;

static DisplayModeIndex display_mode_index;


void register_map();

// TODO: should pass in config struct...
void map_init() {
    // quickpix
    quickpix_init(&quickpix);
    quickpix_add_chain(&quickpix, 6, 618);
    quickpix_add_chain(&quickpix, 7, 548);

    // pxg
    pxg_init(&pixel_group);
    for (unsigned i = 0; i < quickpix.count; i++) {
        pxg_add_chain(&pixel_group, quickpix.chains[i].count);
    }

    register_map();
    dsp_init_presets();
}

void map_update(int64_t frame_time_us) {
    const DisplayData* data = dsp_get_preset(display_mode_index);
    switch (data->mode) {
        case DISPLAY_MODE_SOLID: {
            pxg_apply(&pixel_group, dsp_mode_solid, (void*)data);
            break;
        }
        case DISPLAY_MODE_TRAIN_LINE_COLORS: {
            pxg_apply(&pixel_group, dsp_mode_train_line_colors, (void*)data);
            break;
        }
        case DISPLAY_MODE_WAVE: {
            pxg_apply(&pixel_group, dsp_mode_wave, (void*)data);
            dsp_mode_wave_tick((void*)data, frame_time_us);
            break;
        }
        case DISPLAY_MODE_SIMULATE_TRAINS: {
            pxg_apply(&pixel_group, dsp_mode_simulate_trains, (void*)data);
            dsp_mode_simulate_trains_tick((void*)data, frame_time_us);
            break;
        }
        case DISPLAY_MODE_PULSE: {
            pxg_apply(&pixel_group, dsp_mode_pulse, (void*)data);
            dsp_mode_pulse_tick((void*)data, frame_time_us);
            break;
        }
    }
}

void map_display() {
    // update hardware pixel array
    for (unsigned chain_idx = 0; chain_idx < pixel_group.count; chain_idx++) {
        for (PixelIndex px_idx = 0; px_idx < pixel_group.chains[chain_idx].count; px_idx++) {
            // TODO also apply limits? (e.g. max brightness, some colorspace weirdness?)
            Color c = pixel_group.chains[chain_idx].data[px_idx].color;
            c.r /= 2;
            c.g /= 2;
            c.b /= 2;
            quickpix.chains[chain_idx].pixels[px_idx] = color_to_hardware_format(c);
        }
    }

    quickpix_show(&quickpix);
}

void map_clear() {
    for (unsigned chain_idx = 0; chain_idx < pixel_group.count; chain_idx++) {
        for (PixelIndex px_idx = 0; px_idx < pixel_group.chains[chain_idx].count; px_idx++) {
            quickpix.chains[chain_idx].pixels[px_idx] = (uint32_t) 0;
        }
    }

    quickpix_show(&quickpix);
}

void map_next_display_mode() {
    dsp_next_preset(&display_mode_index);
}

const PixelGroup* map_get_pxg() {
    return &pixel_group;
}

const IntersectionSet* map_get_intx_set() {
    return &intersection_set;
}

const PixelTrainLineMap* map_get_pxtl() {
    return &pixel_train_line_map;
}

void map_get_train_lines(const TrainLine* tl_out, unsigned* tl_cnt) {
    tl_out = train_lines;
    *tl_cnt = train_lines_cnt;
}

// HELPER FUNCTIONS

void register_map() {
    // init globals
    pxtl_init(&pixel_train_line_map);
    intx_set_init(&intersection_set);

    // TODO: parse config struct?
}