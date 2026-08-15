#ifndef MANHATTANMAP_MANHATTANLAYOUT_H
#define MANHATTANMAP_MANHATTANLAYOUT_H

#include "Pixel.h"
#include "TrainLine.h"

// quick c-style vector2 float cuz I'm lazy
#define CVEC2F(x, y) (const float[2]){ x, y }

inline void manhattan_register_pixels(PixelGroup* pixel_group) {
    pxg_init(pixel_group);

    PixelHandle px = (PixelHandle) {
        .index = 0,
        .chain = 0,
        .group = pixel_group,
    };

    px.chain = 0;

    px.index = 0;
    px_set_strip_positions(px, 10, CVEC2F(.1f, .1f), CVEC2F(.2f, .2f));
    px.index = 11;
    px_set_strip_positions(px, 20, CVEC2F(.1f, .1f), CVEC2F(.2f, .2f));
    // TODO... etc. etc.
}

inline void manhattan_register_trainlines(PixelGroup* pixel_group, PixelTrainLineMap* pixel_train_line_map, IntersectionSet* intersection_set) {
    PixelHandle px1 = (PixelHandle) {
        .index = 0,
        .chain = 0,
        .group = pixel_group,
    };
    PixelHandle px2 = px1;

    TrainLine tl_1;
    tl_init(&tl_1, LINE_1);
    px1.index = 0; px2.index = 10;
    tl_add_segment(&tl_1, px1, px2, pixel_train_line_map);
    px1.index = 11; px2.index = 20;
    tl_add_segment(&tl_1, px1, px2, pixel_train_line_map);
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
    Intersection intx = intx_create_empty();
    intx.pixel.index = 20;
    intx_assign_train_lines(&intx, 3, tl_1, tl_2, tl_3);
    intx_set_insert(intersection_set, intx);
}


#endif //MANHATTANMAP_MANHATTANLAYOUT_H