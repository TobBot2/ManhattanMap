#include "Pixel.h"

#include <math.h>

// Color color_from(uint8_t r, uint8_t g, uint8_t b) {
//     return (Color){0, g, r, b};
// }

Color color_mix(Color c1, Color c2, float mix) {
    // mix colors
    Color c1_contribution = color_from_rgb(
        c1.r * (1.f - mix),
        c1.g * (1.f - mix),
        c1.b * (1.f - mix)
    );
    Color c2_contribution = color_from_rgb(
        c2.r * mix,
        c2.g * mix,
        c2.b * mix
    );
    return color_from_rgb (
        c1_contribution.r + c2_contribution.r,
        c1_contribution.g + c2_contribution.g,
        c1_contribution.b + c2_contribution.b
    );
}

// ******************************************************************************
//                                 PIXEL GROUP
// ******************************************************************************

void pxg_init(PixelGroup* pxg, const Quickpix* quickpix) {
    static_assert(sizeof(*quickpix->pixels) == sizeof(Color), "pixel datatype should be compatible with Color");

    pxg->positions = malloc(quickpix->pixels_count * sizeof(*pxg->positions));
    pxg->colors = (Color*)quickpix->pixels; // connect hardware pixels to pxg->colors
    pxg->count = quickpix->pixels_count;
}

void pxg_apply(const PixelGroup* pxg, PixelTransformFn fn, void* aux) {
    for (PixelIndex i = 0; i < pxg->count; i++) {
        fn(pxg, i, aux);
    }
}

// ******************************************************************************
//                                   PIXELS
// ******************************************************************************

void px_set_color(const PixelGroup* pxg, PixelIndex px, Color color) {
    pxg->colors[px] = color;
}
Color px_get_color(const PixelGroup* pxg, PixelIndex px) {
    return pxg->colors[px];
}

void px_set_position(const PixelGroup* pxg, PixelIndex px, float x, float y) {
    pxg->positions[px][0] = x;
    pxg->positions[px][1] = y;
}

void px_get_position(const PixelGroup* pxg, PixelIndex px, float* out_x, float* out_y) {
    *out_x = pxg->positions[px][0];
    *out_y = pxg->positions[px][1];
}
