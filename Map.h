#ifndef MANHATTANMAP_MANHATTANMAP_H
#define MANHATTANMAP_MANHATTANMAP_H

#include "quickpix.h"

#include "Pixel.h"
#include "TrainLine.h"

// MAP LOGIC

typedef struct Map_t {
    PixelGroup* pxg;
    PixelTrainLines* pxtl;
    TrainLine* tls;
    TrainLineIndex tls_count;
} Map;

void map_init(Map* map, const Quickpix* quickpix);
void map_clear(const Map* map);

PixelIndex map_get_random_next_neighbor(const Map* map, PixelIndex pixel, PixelIndex prev);

#endif //MANHATTANMAP_MANHATTANMAP_H