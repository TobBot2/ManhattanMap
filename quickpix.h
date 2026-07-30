// single header lib for dealing with ws2812 pixels on pico

#ifndef MANHATTANMAP_WS2812_H
#define MANHATTANMAP_WS2812_H

#include "hardware/pio.h"
#include "pico/types.h"

typedef struct Quickpix_t {
    // ws2812 stuff
    PIO pio;
    uint sm;
    uint offset;

    // cool data
    uint32_t* pixels;
    uint count;
} Quickpix;

static bool quickpix_init(Quickpix* quickpix, uint pin, uint count) {
    quickpix->count = count;
    if (!pio_claim_free_sm_and_add_program_for_gpio_range(
            &ws2812_program, &quickpix->pio, &quickpix->sm, &quickpix->offset, pin, 1, true)) {
        return false;
    }
    ws2812_program_init(quickpix->pio, quickpix->sm, quickpix->offset, pin, 800000, false);
    return true;
}

static void quickpix_show(Quickpix* quickpix) {
    for (uint i = 0; i < quickpix->count; i++)
        pio_sm_put_blocking(quickpix->pio, quickpix->sm, quickpix->pixels[i] << 8u);
}

static uint32_t quickpix_rgb(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)g << 16) | ((uint32_t)r << 8) | (uint32_t)b;
}

#endif //MANHATTANMAP_WS2812_H