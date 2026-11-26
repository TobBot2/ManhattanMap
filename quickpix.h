// single header lib for dealing raw ws2812 pixels on pico

#ifndef MANHATTANMAP_WS2812_H
#define MANHATTANMAP_WS2812_H

#include <stdio.h>
#include <stdlib.h>

#include "hardware/pio.h"
#include "pico/time.h"
#include "pico/types.h"
#include "ws2812.pio.h"

// index into `uint32_t* pixels` array (negative = invalid)
typedef int16_t PixelIndex;

typedef struct QuickpixChain_t {
    // ws2812 stuff
    PIO pio;
    uint sm;
    uint offset;

    PixelIndex start_idx; // idx in pixels arr
    PixelIndex pixels_count;
} QuickpixChain;

typedef struct Quickpix_t {
    QuickpixChain* chains;
    unsigned chains_count;

    uint32_t* pixels;
    PixelIndex pixels_count;
} Quickpix;

static void quickpix_init(Quickpix* quickpix) {
    quickpix->chains = NULL;
    quickpix->chains_count = 0;
    quickpix->pixels = NULL;
    quickpix->pixels_count = 0;
}

static bool quickpix_add_chain(Quickpix* quickpix, uint pin, uint pixels_count) {
    // allocate new chain
    quickpix->chains_count++;
    quickpix->chains = realloc(quickpix->chains, quickpix->chains_count * sizeof(*quickpix->chains));
    QuickpixChain* chain = &quickpix->chains[quickpix->chains_count - 1];

    // populate new chain data
    chain->start_idx = quickpix->pixels_count;
    chain->pixels_count = pixels_count;
    quickpix->pixels_count += pixels_count;
    quickpix->pixels = realloc(quickpix->pixels, quickpix->pixels_count * sizeof(*quickpix->pixels));
    if (!pio_claim_free_sm_and_add_program_for_gpio_range(
            &ws2812_program, &chain->pio, &chain->sm, &chain->offset, pin, 1, true)) {
        return false;
    }
    ws2812_program_init(chain->pio, chain->sm, chain->offset, pin, 800000, false);

    return true;
}

static void quickpix_show(const Quickpix* quickpix) {
    for (unsigned c = 0; c < quickpix->chains_count; c++) {
        QuickpixChain* chain = &quickpix->chains[c];
        for (PixelIndex i = 0; i < chain->pixels_count; i++) {
            pio_sm_put_blocking(chain->pio, chain->sm, quickpix->pixels[chain->start_idx + i]);
        }
    }
    sleep_us(100); // ensure protocol latch (minimum 50 us)
}

#endif //MANHATTANMAP_WS2812_H