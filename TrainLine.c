#include "TrainLine.h"

#include <stdio.h>

#include "Map.h"

// ******************************************************************************
//                                 TRAIN LINE
// ******************************************************************************

void tl_init(TrainLine* tl, Color color) {
    tl->color = color;
    tl->pixels = NULL;
    tl->pixels_count = 0;
}

void tl_add_segment(uint8_t tl_idx, PixelIndex start, PixelIndex end, TrainLine* tls, const PixelTrainLines* pxtl) {
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

PixelIndex tl_get_neighbor(const TrainLine* tl, unsigned* pixel_idx, int forward) {
    if (forward) *pixel_idx += 1;
    else *pixel_idx -= 1;

    if (*pixel_idx >= tl->pixels_count) {
        return -1;
    }

    return tl->pixels[*pixel_idx];
}

PixelIndex tl_get_next_neighbor(const TrainLine* tl, unsigned* pixel_idx, unsigned* prev_idx) {
    int forward = *pixel_idx > *prev_idx;
    *prev_idx = *pixel_idx;
    return tl_get_neighbor(tl, pixel_idx, forward);
}

static void add_unique(PixelIndex* list, unsigned* n, PixelIndex v, PixelIndex prev) {
    for (unsigned i = 0; i < *n; i++)
        if (list[i] == v || v == prev) return; // already present
    list[(*n)++] = v;
}

PixelIndex tl_get_random_next_neighbor(PixelIndex pixel, PixelIndex prev, const PixelTrainLines* pxtl, const TrainLine* tls) {
    PixelIndex candidates[64];
    unsigned n = 0;

    // visit only the lines this pixel belongs to (set bits in the mask)
    for (uint32_t m = pxtl->tl_masks[pixel]; m; m &= m - 1) {
        const TrainLine* tl = &tls[__builtin_ctz(m)]; // mask to tl_index to tl

        // linear search for pixel index in trainline
        for (unsigned i = 0; i < tl->pixels_count; i++) {
            if (tl->pixels[i] != pixel) continue;

            // found pixel index, add candidate if in bounds
            if (i > 0) add_unique(candidates, &n, tl->pixels[i - 1], prev);
            if (i + 1 < tl->pixels_count) add_unique(candidates, &n, tl->pixels[i + 1], prev);
            break; // found this pixel in this line
        }
    }

    // no valid neighbors
    if (n == 0) return -1;

    return candidates[rand() % n]; // pick from candidates at random
}

// ******************************************************************************
//                              PIXEL TRAINLINE MAP
// ******************************************************************************

void pxtl_init(PixelTrainLines* pxtl, const PixelGroup* pxg) {
    pxtl->tl_masks = calloc(pxg->count, sizeof(*pxtl->tl_masks));
    // pxtl->tls_count
    pxtl->pixels_count = pxg->count;
}

void pxtl_associate_line(const PixelTrainLines* pxtl, PixelIndex px, uint8_t tl_idx) {
    pxtl->tl_masks[px] |= (uint32_t)1 << tl_idx;
}