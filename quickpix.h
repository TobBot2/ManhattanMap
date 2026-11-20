// single header lib for dealing raw ws2812 pixels on pico

#ifndef MANHATTANMAP_WS2812_H
#define MANHATTANMAP_WS2812_H

#include <stdlib.h>

#include "hardware/pio.h"
#include "pico/time.h"
#include "pico/types.h"
#include "ws2812.pio.h"

typedef struct QuickpixChain_t {
    // ws2812 stuff
    PIO pio;
    uint sm;
    uint offset;

    // cool data
    uint32_t* pixels;
    uint count;
} QuickpixChain;

typedef struct Quickpix_t {
    QuickpixChain* chains;
    unsigned count;
} Quickpix;

static void quickpix_init(Quickpix* quickpix) {
    quickpix->chains = NULL;
    quickpix->count = 0;
}

static bool quickpix_add_chain(Quickpix* quickpix, uint pin, uint count) {
    // allocate new chain
    quickpix->count++;
    quickpix->chains = realloc(quickpix->chains, quickpix->count * sizeof(QuickpixChain));
    QuickpixChain* chain = &quickpix->chains[quickpix->count - 1];

    // populate new chain data
    chain->count = count;
    chain->pixels = calloc(count, sizeof(uint32_t));
    if (!pio_claim_free_sm_and_add_program_for_gpio_range(
            &ws2812_program, &chain->pio, &chain->sm, &chain->offset, pin, 1, true)) {
        return false;
    }
    ws2812_program_init(chain->pio, chain->sm, chain->offset, pin, 800000, false);

    return true;
}

static void quickpix_show(const Quickpix* quickpix) {
    for (unsigned c = 0; c < quickpix->count; c++) {
        QuickpixChain* chain = &quickpix->chains[c];
        for (uint i = 0; i < chain->count; i++) {
            pio_sm_put_blocking(chain->pio, chain->sm, chain->pixels[i] << 8u);
        }
    }
    sleep_us(100); // ensure protocol latch (minimum 50 us)
}

#endif //MANHATTANMAP_WS2812_H