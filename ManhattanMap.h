#ifndef MANHATTANMAP_MANHATTANMAP_H
#define MANHATTANMAP_MANHATTANMAP_H

#include "pico/types.h"

#include "Pixel.h"
#include "TrainLine.h"

// MAP LOGIC

void map_init();
void map_update(int64_t frame_time_us);
void map_display();
void map_clear();

void map_next_display_mode();

// expose some vars for special use cases
const PixelGroup* map_get_pxg();
const IntersectionSet* map_get_intx_set();
const PixelTrainLineMap* map_get_pxtl();
void map_get_train_lines(const TrainLine* tl_out, unsigned* tl_cnt);

#endif //MANHATTANMAP_MANHATTANMAP_H