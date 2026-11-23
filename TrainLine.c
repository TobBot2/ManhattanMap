#include "TrainLine.h"

#include <stdio.h>

// ******************************************************************************
//                                 TRAIN LINE
// ******************************************************************************

void tl_init(TrainLine* tl, Color color) {
    tl->color = color;
    tl->pixels = NULL;
    tl->pixels_count = 0;
}

void tl_add_segment(TrainLineIndex tl_idx, PixelIndex start, PixelIndex end, TrainLine* tls, const PixelTrainLines* pxtl) {
    TrainLine* tl = &tls[tl_idx];
    PixelIndex original_cnt = tl->pixels_count;

    if (start > end) return; // end must be after start

    // reserve space
    unsigned extra_space = end - start + 1; // + 1 for inclusive range [start, end]
    tl->pixels_count += extra_space;
    tl->pixels = realloc(tl->pixels, tl->pixels_count * sizeof(*tl->pixels));

    // populate space
    for (PixelIndex i = 0; i < extra_space; i++) {
        tl->pixels[original_cnt + i] = start + i;
        pxtl_associate_line(pxtl, tl->pixels[original_cnt + i], tl_idx);
    }
}

PixelIndex tl_get_nth_next_pixel(const TrainLine* tl, unsigned* pixel_idx, int n) {
    *pixel_idx += n;

    if (*pixel_idx >= tl->pixels_count) {
        return -1;
    }

    return tl->pixels[*pixel_idx];
}

PixelIndex tl_get_next_pixel(const TrainLine* tl, unsigned* pixel_idx, unsigned* prev_idx) {
    int forward = *pixel_idx - *prev_idx;
    *prev_idx = *pixel_idx;
    return tl_get_nth_next_pixel(tl, pixel_idx, forward);
}

// ******************************************************************************
//                              PIXEL TRAINLINE MAP
// ******************************************************************************

void pxtl_init(PixelTrainLines* pxtl, const PixelGroup* pxg, TrainLineIndex tls_count) {
    pxtl->tl_masks = calloc(pxg->count, sizeof(*pxtl->tl_masks));
    pxtl->tls_count = tls_count;
    pxtl->pixels_count = pxg->count;
}

void pxtl_associate_line(const PixelTrainLines* pxtl, PixelIndex px, TrainLineIndex tl_idx) {
    pxtl->tl_masks[px] |= (uint32_t)1 << tl_idx;
}