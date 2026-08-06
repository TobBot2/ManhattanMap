#ifndef MANHATTANMAP_MANHATTANMAP_H
#define MANHATTANMAP_MANHATTANMAP_H

#include "pico/types.h"

#include "Pixel.h"

// MAP LOGIC

void map_init();
void map_update();
void map_display();

void map_next_display_mode_cb(uint pin, uint32_t events);

// expose some vars for special use cases
PixelGroup* map_get_pxg();

#endif //MANHATTANMAP_MANHATTANMAP_H