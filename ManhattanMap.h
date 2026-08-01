#ifndef MANHATTANMAP_MANHATTANMAP_H
#define MANHATTANMAP_MANHATTANMAP_H

#include "pico/types.h"

// MAP LOGIC

void map_init();
void map_update();
void map_display();

void map_next_display_mode_cb(uint pin, uint32_t events);

#endif //MANHATTANMAP_MANHATTANMAP_H