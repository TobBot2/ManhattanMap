#ifndef MANHATTANMAP_DISPLAYMODE_H
#define MANHATTANMAP_DISPLAYMODE_H

#include "PixelChain.h"

enum DisplayMode {
    _DISPLAY_MODE_BEGIN,
    DISPLAY_MODE_SOLID,
    DISPLAY_MODE_TRAIN_LINE_COLORS,
    DISPLAY_MODE_GRADIENT,
    DISPLAY_MODE_PULSE,
    DISPLAY_MODE_SIMULATE_TRAINS,
    _DISPLAY_MODE_END
};

typedef union {
    Color color;

    struct {
        Color color1;
        Color color2;
        float x1, y1;
        float x2, y2;
    } gradient;

    struct {
        Color color;
        float x, y;
        float speed;
    } pulse;

    struct {
        int count;
        float speed;
    } simulate;
} DisplayData;

#endif //MANHATTANMAP_DISPLAYMODE_H
