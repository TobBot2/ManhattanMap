#ifndef MANHATTANMAP_MANHATTANMAP_H
#define MANHATTANMAP_MANHATTANMAP_H

#include "Pixel.h"
#include "TrainLine.h"
#include "quickpix.h"

// MAP LOGIC

void map_init(const Quickpix* quickpix);
void map_update(int64_t frame_time_us);
void map_clear();

void map_next_display_mode();

// expose some vars for special use cases
const PixelGroup* map_get_pxg();
const PixelTrainLines* map_get_pxtl_map();
void map_get_train_lines(const TrainLine** tl_out, unsigned* tl_cnt);

#endif //MANHATTANMAP_MANHATTANMAP_H