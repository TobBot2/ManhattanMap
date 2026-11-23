#include <stdio.h>
#include <time.h>

#include "hardware/gpio.h"
#include "pico/stdio.h"
#include "pico/time.h"
#include "pico/types.h"

#include "config.h"
#include "DisplayMode.h"
#include "Map.h"
#include "quickpix.h"

// mode to see what physical pixel each handle corresponds to.
// check stdout for current handle (chain + index) of lit up pixel
//#define SETUP_MODE

static Quickpix quickpix;
static Map map;
static DisplayModeIndex display_mode_index;

static PixelIndex debugLight = 0;

static void initialize_io(Quickpix* qpix);
static void button_cb(uint pin, uint32_t events);
static volatile bool button_pressed = false;

[[noreturn]]
int main(void) {
    initialize_io(&quickpix);

    map_init(&map, &quickpix);
    dsp_init_presets(&map);

    map_clear(&map);

    printf("Starting main loop...");
    const int64_t target_frame_time_us = 1000000 / CONFIG.target_fps;
    while (true) {
#ifdef SETUP_MODE
        // callback forcefully updates map already by accessing pixel handle directly
        if (button_pressed) {
            button_pressed = false;

            px_set_color(map.pxg, debugLight, (Color){0, 0, 0, 0});
            for (int i = 0; i < 1; i++) debugLight = (debugLight + 1) % map.pxg->count;
            px_set_color(map.pxg, debugLight, (Color){0, 0, 255, 0});

            printf("PixelIndex: %d\n", debugLight);
            quickpix_show(&quickpix);
        }
        sleep_ms(50); // sleep a little bit just cuz (no need for super fast update in debug)
#else
        absolute_time_t frame_start = get_absolute_time();

        // MAIN LOGIC ---------------------------------------------------------------------

        // handle input
        if (button_pressed) { dsp_next_preset(&display_mode_index); }
        // update state
        dsp_update_map(&map, dsp_get_preset(display_mode_index), target_frame_time_us);
        // render
        quickpix_show(&quickpix);

        // END MAIN LOGIC -----------------------------------------------------------------

        absolute_time_t frame_end = get_absolute_time();

        int64_t frame_time_us = absolute_time_diff_us(frame_start, frame_end);
        int64_t remaining_time_us = target_frame_time_us - frame_time_us;
        if (remaining_time_us > 0) {
            sleep_us((uint64_t)remaining_time_us);
        }
#endif
    }
}

static void initialize_io(Quickpix* qpix) {
    // init debug onboard LED
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    stdio_init_all();

    // execute light-flashing sequence
    for (int i = 0; i < 10; i++) {
        gpio_put(PICO_DEFAULT_LED_PIN, true);
        sleep_ms(50);
        gpio_put(PICO_DEFAULT_LED_PIN, false);
        sleep_ms(50);
    }

    sleep_ms(4000);

    // GPIO
    gpio_init(CONFIG.button.pin);
    gpio_pull_up(CONFIG.button.pin);
    gpio_set_irq_enabled_with_callback(CONFIG.button.pin, GPIO_IRQ_EDGE_FALL, true, &button_cb);

    // quickpix
    quickpix_init(qpix);
    for (unsigned i = 0; i < CONFIG.chains_count; i++) {
        const ChainConfig* chain = &CONFIG.chain_configs[i];
        quickpix_add_chain(qpix, chain->pin, chain->leds_count);
    }
}

static void button_cb(uint pin, uint32_t events) {
    static uint32_t last_time = 0;
    uint32_t now = to_ms_since_boot(get_absolute_time());
    if (now - last_time < CONFIG.button.debounce_ms) return;
    last_time = now;
    button_pressed = true;
}