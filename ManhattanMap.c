#include "ManhattanMap.h"

#include <stdio.h>

#include "DisplayMode.h"
#include "quickpix.h"
#include "TrainLine.h"
#include "ManhattanLayout.h"

Quickpix quickpix;
PixelGroup pixel_group;
PixelTrainLineMap pixel_train_line_map;
DisplayModeIndex display_mode_index;
IntersectionSet intersection_set;

void register_map();

// TODO: should pass in config struct...
void map_init() {
    quickpix = quickpix_create();
    pxg_init(&pixel_group); // TODO: standardize convention (= create() or init()?) I think I prefer init().

    quickpix_add_chain(&quickpix, 6, 8);
    quickpix_add_chain(&quickpix, 7, 34);
    for (unsigned i = 0; i < quickpix.count; i++) {
        pxg_add_chain(&pixel_group, quickpix.chains[i].count);
    }

    register_map();
}

void map_update() {
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
            break;
        }
        case DISPLAY_MODE_SIMULATE_TRAINS: {
            break;
        }
        case DISPLAY_MODE_PULSE:
            break;
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

void map_next_display_mode_cb(uint pin, uint32_t events) {
    dsp_next_preset(&display_mode_index);
}

PixelGroup* map_get_pxg() {
    return &pixel_group;
}

// HELPER FUNCTIONS

void register_map() {
    // init globals
    pxg_init(&pixel_group);
    pxtl_init(&pixel_train_line_map);
    intx_set_init(&intersection_set);

    // call map-specific registry setup
    manhattan_register_pixels(&pixel_group);
    manhattan_register_trainlines(&pixel_group, &pixel_train_line_map, &intersection_set);
}