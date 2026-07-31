#ifndef MANHATTANMAP_MANHATTANMAP_H
#define MANHATTANMAP_MANHATTANMAP_H

#include "TrainLine.h"
#include "DisplayMode.h"
#include "quickpix.h"

// MANHATTAN MAP GLOBALS

extern Quickpix quickpix; // hardware abstraction
extern PixelGroup pixel_group; // logical pixels container
extern PixelTrainLineMap pixel_train_line_map; // pixel -> trainline lookup map
extern DisplayModeIndex display_mode_index; // index into display mode presets arr
extern IntersectionSet intersection_set; // intersections (retrieved by pixel)

extern float max_brightness;

// MAP LOGIC

void map_init();

// QUICKPIX INTERFACE

// mirror pixel_group chains to quickpix chains
void map_link_quickpix();
// update quickpix pixels to pixel_group pixels (hardware encoded)
void map_update_quickpix();

#endif //MANHATTANMAP_MANHATTANMAP_H