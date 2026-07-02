#include "TrainLine.h"

void tl_init(TrainLine* tl, TrainLineName line) {
    tl->line = line;
    tl->pixel_handles = NULL;
    tl->pixels_cnt = 0;
}

// add one pixel to train line
void tl_add_pixels(TrainLine* tl, PixelHandle* pixels, unsigned count) {
    PixelIndex original_cnt = tl->pixels_cnt;

    tl->pixels_cnt += count;
    tl->pixel_handles = realloc(tl->pixel_handles, tl->pixels_cnt);
    for (int i = 0; i < count; i++) {
        tl->pixel_handles[original_cnt + i] = pixels[i];
    }
}

// add contiguous segment of pixels to train line in range [start, end]
void tl_add_segment(TrainLine* tl, PixelGroup* pixel_group, PixelHandle start, PixelHandle end) {
    PixelIndex original_cnt = tl->pixels_cnt;

    // reserve space
    tl->pixels_cnt += end - start + 1; // + 1 for inclusive range
    tl->pixel_handles = realloc(tl->pixel_handles, tl->pixels_cnt);

    // populate space
    for (PixelIndex i = 0; i <= end - start; i++) { // inclusive range [start, end]
        tl->pixel_handles[original_cnt + i].chain = strip;
        tl->pixel_handles[original_cnt + i].index = start + i;
    }
}

Color getTrainLineColor(TrainLineName line) {
    switch (line) {
        case LINE_1:
        case LINE_2:
        case LINE_3:
            return (Color){ .r = 0xEE, .g = 0x35, .b = 0x2E };

        case LINE_A:
        case LINE_C:
        case LINE_E:
            return (Color){ .r = 0x00, .g = 0x39, .b = 0xA6 };

        case LINE_4:
        case LINE_5:
        case LINE_6:
            return (Color){ .r = 0x00, .g = 0x93, .b = 0x3C };

        case LINE_B:
        case LINE_D:
        case LINE_F:
        case LINE_M:
            return (Color){ .r = 0xFF, .g = 0x63, .b = 0x19 };

        case LINE_Q:
        case LINE_N:
        case LINE_R:
        case LINE_W:
            return (Color){ .r = 0xFC, .g = 0xCC, .b = 0x0A };

        case LINE_7:
            return (Color){ .r = 0xB9, .g = 0x33, .b = 0xAD };

        case LINE_L:
            return (Color){ .r = 0xA7, .g = 0xA9, .b = 0xAC };

        case LINE_S:
            return (Color){ .r = 0x80, .g = 0x81, .b = 0x83 };

        default:
            return (Color){ .r = 0x00, .g = 0x00, .b = 0x00 };
    }
}