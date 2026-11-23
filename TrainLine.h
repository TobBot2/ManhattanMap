#ifndef MANHATTANMAP_TRAINLINE_H
#define MANHATTANMAP_TRAINLINE_H

#include <stdlib.h>

#include "Pixel.h"

typedef int8_t TrainLineIndex;

typedef struct TrainLine_t {
    Color color;
    PixelIndex* pixels;
    PixelIndex pixels_count;
} TrainLine;

typedef struct PixelTrainLines_t {
    uint32_t* tl_masks;
    TrainLineIndex tls_count;
    PixelIndex pixels_count;
} PixelTrainLines;

// TRAIN LINE

void tl_init(TrainLine* tl, Color color);
PixelIndex tl_get_nth_next_pixel(const TrainLine* tl, unsigned* pixel_idx, int n);
PixelIndex tl_get_next_pixel(const TrainLine* tl, unsigned* pixel_idx, unsigned* prev_idx);

// PIXEL-TRAINLINES

void pxtl_init(PixelTrainLines* pxtl, const PixelGroup* pxg, TrainLineIndex tls_count);
void pxtl_associate_line(const PixelTrainLines* pxtl, PixelIndex px, TrainLineIndex tl_idx);
uint32_t* pxtl_get_mask(const PixelTrainLines* pxtl, PixelIndex px);

#endif //MANHATTANMAP_TRAINLINE_H