#ifndef MANHATTANMAP_TRAINLINE_H
#define MANHATTANMAP_TRAINLINE_H

#include <stdlib.h>

#include "Pixel.h"
#include "AvlTree.h"

// TODO: hide implementation of TrainLine, requiring tl_* funcs
//  for access/manipulation?

typedef struct TrainLine_t {
    Color color;
    PixelIndex* pixels;
    PixelIndex pixels_count;
} TrainLine;

typedef struct PixelTrainLines_t {
    uint32_t* tl_masks;
    uint8_t tls_count;
    PixelIndex pixels_count;
} PixelTrainLines;

// TRAIN LINE

void tl_init(TrainLine* tl, Color color);
// add contiguous segment of pixels to train line in range [start, end]
void tl_add_segment(uint8_t tl_idx, PixelIndex start, PixelIndex end, TrainLine* tls, const PixelTrainLines* pxtl);
// get neighboring pixel along train line (from train line pixels array index)
PixelIndex tl_get_neighbor(const TrainLine* tl, unsigned* pixel_idx, int forward);
// get neighboring pixel in opposite direction of previous (neighbor in continuing direction)
PixelIndex tl_get_next_neighbor(const TrainLine* tl, unsigned* pixel_idx, unsigned* prev_idx);
// get neighboring pixel on any train line in any direction except previous (don't back-track)
PixelIndex tl_get_random_next_neighbor(PixelIndex pixel, PixelIndex prev, const PixelTrainLines* pxtl, const TrainLine* tls);

// PIXEL TRAINLINE MAP

void pxtl_init(PixelTrainLines* pxtl, const PixelGroup* pxg);
void pxtl_associate_line(const PixelTrainLines* pxtl, PixelIndex px, uint8_t tl_idx);
uint32_t* pxtl_get_mask(const PixelTrainLines* pxtl, PixelIndex px);

#endif //MANHATTANMAP_TRAINLINE_H