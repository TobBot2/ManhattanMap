#include "ManhattanMap.h"

#include <stdio.h>

#include "DisplayMode.h"
#include "quickpix.h"
#include "TrainLine.h"

// quick c-style vector2 float cuz I'm lazy
#define cvec2f(x, y) (const float[2]){ x, y }

Quickpix quickpix;
PixelGroup pixel_group;
PixelTrainLineMap pixel_train_line_map;
DisplayModeIndex display_mode_index;
IntersectionSet intersection_set;

void register_pixels();
void register_trainlines();

// TODO: should pass in config struct...
void map_init() {
    quickpix = quickpix_create();
    pxg_init(&pixel_group); // TODO: standardize convention (= create() or init()?) I think I prefer init().

    quickpix_add_chain(&quickpix, 6, 542);
    quickpix_add_chain(&quickpix, 7, 662);
    for (unsigned i = 0; i < quickpix.count; i++) {
        pxg_add_chain(&pixel_group, quickpix.chains[i].count);
    }

    register_pixels();
    register_trainlines();
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
            quickpix.chains[chain_idx].pixels[px_idx] = color_to_hardware_format(pixel_group.chains[chain_idx].data[px_idx].color);
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

void register_pixels() {
    pxg_init(&pixel_group);

    PixelHandle px = (PixelHandle) {
        .index = 0,
        .chain = 0,
        .group = &pixel_group,
    };

    px.chain = 0;

    px.index = 0;
    px_set_strip_positions(px, 10, cvec2f(.1f, .1f), cvec2f(.2f, .2f));
    px.index = 11;
    px_set_strip_positions(px, 20, cvec2f(.1f, .1f), cvec2f(.2f, .2f));
    // TODO... etc. etc.
}

void register_trainlines() {
    PixelHandle px1 = (PixelHandle) {
        .index = 0,
        .chain = 0,
        .group = &pixel_group,
    };
    PixelHandle px2 = px1;

    TrainLine tl_1;
    tl_init(&tl_1, LINE_1);
    px1.index = 0; px2.index = 10;
    tl_add_segment(&tl_1, px1, px2, &pixel_train_line_map);
    px1.index = 11; px2.index = 20;
    tl_add_segment(&tl_1, px1, px2, &pixel_train_line_map);
    // TODO... etc. etc.

    TrainLine tl_2;
    tl_init(&tl_2, LINE_2);
    TrainLine tl_3;
    tl_init(&tl_3, LINE_3);

    TrainLine tl_A;
    tl_init(&tl_A, LINE_A);
    TrainLine tl_C;
    tl_init(&tl_C, LINE_C);
    TrainLine tl_E;
    tl_init(&tl_E, LINE_E);

    TrainLine tl_B;
    tl_init(&tl_B, LINE_B);
    TrainLine tl_D;
    tl_init(&tl_D, LINE_D);
    TrainLine tl_F;
    tl_init(&tl_F, LINE_F);
    TrainLine tl_M;
    tl_init(&tl_M, LINE_M);

    TrainLine tl_4;
    tl_init(&tl_4, LINE_4);
    TrainLine tl_5;
    tl_init(&tl_5, LINE_5);
    TrainLine tl_6;
    tl_init(&tl_6, LINE_6);

    TrainLine tl_Q;
    tl_init(&tl_Q, LINE_Q);
    TrainLine tl_N;
    tl_init(&tl_N, LINE_N);
    TrainLine tl_R;
    tl_init(&tl_R, LINE_R);
    TrainLine tl_W;
    tl_init(&tl_W, LINE_W);

    TrainLine tl_7;
    tl_init(&tl_7, LINE_7);
    TrainLine tl_L;
    tl_init(&tl_L, LINE_L);
    TrainLine tl_S;
    tl_init(&tl_S, LINE_S);

    // TODO intersections
    intx_set_init(&intersection_set);
    Intersection intx = intx_create_empty();
    intx.pixel.index = 20;
    intx_assign_train_lines(&intx, 3, tl_1, tl_2, tl_3);
    intx_set_insert(&intersection_set, intx);
}