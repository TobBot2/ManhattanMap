#include "ManhattanMap.h"

#include <stdio.h>

static void register_pixels();
static void register_trainlines();

void map_init() {
    pxg_init(&pixel_group);

    register_pixels();
    register_trainlines();
}

void map_link_quickpix() {
    for (unsigned i = 0; i < quickpix.count; i++) {
        pxg_add_chain(&pixel_group, quickpix.chains[i].count);
    }
}

void map_update_quickpix() {
    for (unsigned chain_idx = 0; chain_idx < pixel_group.count; chain_idx++) {
        for (PixelIndex px_idx = 0; px_idx < pixel_group.chains[chain_idx].count; px_idx++) {
            // TODO also apply limits? (e.g. max brightness, some colorspace weirdness?)
            quickpix.chains[chain_idx].pixels[px_idx] = color_to_hardware_format(pixel_group.chains[chain_idx].data[px_idx].color);
        }
    }
}

// HELPER FUNCTIONS

static void register_pixels() {
    PixelHandle px = (PixelHandle) {
        .index = 0,
        .chain = 0,
        .group = &pixel_group,
    };

    px.chain = 0;

    px.index = 0;
    px_set_strip_positions(px, 10, (float[2]){ .1f, .1f }, (float[2]){.2f, .2f});
    px.index = 11;
    px_set_strip_positions(px, 20, (float[2]){ .1f, .1f }, (float[2]){.2f, .2f});
    // TODO... etc. etc.
}

static void register_trainlines() {
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

    intx_set_insert(&intersection_set, intx);
}