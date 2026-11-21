#ifndef MANHATTANMAP_TRAINLINE_H
#define MANHATTANMAP_TRAINLINE_H

#include <stdlib.h>

#include "Pixel.h"
#include "AvlTree.h"

// TODO: hide implementation of TrainLine, requiring tl_* funcs
//  for access/manipulation?

typedef struct TrainLine_t {
    Color color;
    PixelHandle* pixels;
    unsigned pixels_cnt;
} TrainLine;

// avl tree
typedef struct avl_tree PixelTrainLinesMap;
typedef struct PixelTrainLinesNode_t {
    struct avl_node avl;
    PixelHandle key;
    TrainLine** lines;
    unsigned lines_cnt;
} PixelTrainLinesNode;

// TRAIN LINE

void tl_init(TrainLine* tl, Color color);
// add contiguous segment of pixels to train line in range [start, end]
void tl_add_segment(TrainLine* tl, PixelHandle start, PixelHandle end, PixelTrainLinesMap* pxtl_map);
// get neighboring pixel along train line (from train line pixels array index)
PixelHandle tl_get_neighbor(const TrainLine* tl, unsigned* pixel_idx, int forward);
// get neighboring pixel in opposite direction of previous (neighbor in continuing direction)
PixelHandle tl_get_next_neighbor(const TrainLine* tl, unsigned* pixel_idx, unsigned* prev_idx);
// get neighboring pixel on any train line
PixelHandle tl_get_random_neighbor(PixelHandle pixel, const PixelTrainLinesMap* pxtl_map);
// get neighboring pixel on any train line in any direction except previous (don't back-track)
PixelHandle tl_get_random_next_neighbor(PixelHandle pixel, PixelHandle prev, const PixelTrainLinesMap* pxtl_map);

// PIXEL TRAINLINE MAP

void pxtl_map_init(PixelTrainLinesMap* pxtl_map);
void pxtl_map_associate_line(PixelTrainLinesMap* pxtl_map, PixelHandle px, TrainLine* tl);
PixelTrainLinesNode* pxtl_map_search(const PixelTrainLinesMap* pxtl_map, PixelHandle px);

#endif //MANHATTANMAP_TRAINLINE_H