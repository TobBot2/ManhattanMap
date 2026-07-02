#ifndef MANHATTANMAP_TRAINLINE_H
#define MANHATTANMAP_TRAINLINE_H

#include <stdlib.h>

#include "PixelChain.h"

typedef enum {
    _LINE_BEGIN,
    LINE_1,
    LINE_2,
    LINE_3,
    LINE_A,
    LINE_C,
    LINE_E,
    LINE_B,
    LINE_D,
    LINE_F,
    LINE_M,
    LINE_4,
    LINE_5,
    LINE_6,
    LINE_Q,
    LINE_N,
    LINE_R,
    LINE_W,
    LINE_7,
    LINE_L,
    LINE_S,
    _LINE_END
} TrainLineName;

typedef struct TrainLine_t {
    TrainLineName line;
    PixelHandle* pixel_handles;
    unsigned int pixels_cnt;
} TrainLine;

typedef struct Intersection_t {
    PixelHandle pixel;
    TrainLine* lines;
    unsigned int lines_cnt;
} Intersection;

void tl_init(TrainLine* tl, TrainLineName line);

void tl_add_pixels(TrainLine* tl, PixelHandle* pixels, unsigned count);

void tl_add_segment(TrainLine* tl, uint16_t strip, PixelIndex start, PixelIndex end);

Color getTrainLineColor(TrainLineName line);

#endif //MANHATTANMAP_TRAINLINE_H