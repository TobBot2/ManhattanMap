#include "Map.h"

#include <stdio.h>

#include "quickpix.h"
#include "config.h"

#include "DisplayMode.h"

static void map_add_tl_segment(const Map* map, TrainLineIndex tl_idx, PixelIndex start, PixelIndex end) {
    TrainLine* tl = &map->tls[tl_idx];
    PixelIndex original_cnt = tl->pixels_count;

    if (start > end) return; // end must be after start

    // reserve space
    unsigned extra_space = end - start + 1; // + 1 for inclusive range [start, end]
    tl->pixels_count += extra_space;
    tl->pixels = realloc(tl->pixels, tl->pixels_count * sizeof(*tl->pixels));

    // populate space
    for (PixelIndex i = 0; i < extra_space; i++) {
        tl->pixels[original_cnt + i] = start + i;
        pxtl_associate_line(&map->pxtl, tl->pixels[original_cnt + i], tl_idx);
    }
}

void map_init(Map* map, const Quickpix* quickpix) {
    // pixels
    pxg_init(&map->pxg, quickpix);
    for (unsigned s = 0; s < CONFIG.pixel_segments_count; s++) {
        const PixelSegmentConfig* seg = &CONFIG.pixel_segments[s];
        float x = seg->begin_x;
        float y = seg->begin_y;
        float dx = (seg->end_x - seg->begin_x) / (seg->end - seg->begin + 1);
        float dy = (seg->end_y - seg->begin_y) / (seg->end - seg->begin + 1);
        for (PixelIndex p = seg->begin; p <= seg->end; p++) {  // inclusive with end
            px_set_position(&map->pxg, p, x, y);
            x += dx;
            y += dy;
        }
    }

    // pixel-trainlines
    pxtl_init(&map->pxtl, &map->pxg, CONFIG.trainlines_count);

    // train lines
    map->tls_count = CONFIG.trainlines_count;
    map->tls = calloc(map->tls_count, sizeof(TrainLine));
    for (unsigned tl_idx = 0; tl_idx < map->tls_count; tl_idx++) {
        const TrainLineConfig* tl_config = &CONFIG.trainline_configs[tl_idx];
        Color color = color_from_rgb(tl_config->color.r, tl_config->color.g, tl_config->color.b);
        tl_init(&map->tls[tl_idx], color);
        const TrainLineSegmentConfig* seg_end = tl_config->segments + CONFIG.trainlines_count;
        for (const TrainLineSegmentConfig* seg = tl_config->segments; seg != seg_end; seg++) {
            map_add_tl_segment(map, tl_idx, seg->begin, seg->end);
        }
    }
}

void map_clear(const Map* map) {
    for (PixelIndex px = 0; px < map->pxg.count; px++) {
        map->pxg.colors[px] = (Color){ 0 };
    }
}

static void add_unique(PixelIndex* list, unsigned* n, PixelIndex v, PixelIndex prev) {
    for (unsigned i = 0; i < *n; i++)
        if (list[i] == v || v == prev) return; // already present
    list[(*n)++] = v;
}

PixelIndex map_get_random_next_neighbor(const Map* map, PixelIndex pixel, PixelIndex prev) {
    PixelIndex candidates[64];
    unsigned n = 0;

    // visit only the lines this pixel belongs to (set bits in the mask)
    for (uint32_t m = map->pxtl.tl_masks[pixel]; m; m &= m - 1) {
        const TrainLine* tl = &map->tls[__builtin_ctz(m)]; // mask to tl_index (via ctz) to tl (via arr lookup)

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
