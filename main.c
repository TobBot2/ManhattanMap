#include <stdio.h>
#include <time.h>

#include "ManhattanMap.h"
#include "hardware/gpio.h"
#include "pico/stdio.h"
#include "pico/time.h"
#include "pico/types.h"

#include "Pixel.h"

#define BUTTON_PIN 3

// mode to see what physical pixel each handle corresponds to.
// check stdout for current handle (chain + index) of lit up pixel
#define SETUP_MODE

static void initialize_io();
static void sos();

// extra debug stuff
static void debug_next_light_cb(uint pin, uint32_t events);
static PixelHandle debugLight = {
    .chain = 0,
    .index = 0,
};

[[noreturn]]
int main(void) {
    initialize_io();
    map_init();

    const int64_t target_frame_time_us = 1000000 / 60; // 60 fps
    while (true) {
#ifndef SETUP_MODE
        absolute_time_t frame_start = get_absolute_time();
        map_update();
        map_display();
        absolute_time_t frame_end = get_absolute_time();

        int64_t frame_time_us = absolute_time_diff_us(frame_start, frame_end);
        int64_t remaining_time_us = target_frame_time_us - frame_time_us;
        if (remaining_time_us > 0) {
            sleep_us((uint64_t)remaining_time_us);
        }
#else
        // callback forcefully updates map already by accessing pixel handle directly
        map_display();
        sleep_ms(50); // sleep a little bit just cuz (no need for super fast update in debug)
#endif
    }
}

static void initialize_io() {
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

    sleep_ms(2000);

#ifdef SETUP_MODE
    sleep_ms(3000); // give extra time to connect to port for debug serial output
#endif

    // GPIO

#ifndef SETUP_MODE
    gpio_init(BUTTON_PIN);
    gpio_pull_up(BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &map_next_display_mode_cb);
#else
    gpio_init(BUTTON_PIN);
    gpio_pull_up(BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &debug_next_light_cb);
    printf("Initialized debug io.\n");
#endif
}

static void debug_next_light_cb(uint pin, uint32_t events) {
    // ignore multi-presses
    static uint32_t last_time = 0;
    uint32_t now = to_ms_since_boot(get_absolute_time());
    if (now - last_time < 200) return;
    last_time = now;

    // reset prev pixel + set next pixel
    px_set_color(debugLight, (Color){0, 0, 0});
    px_next(&debugLight);
    px_set_color(debugLight, (Color){0, 0, 100});

    printf("\nLight chain: %hu,\tindex: %hu", debugLight.chain, debugLight.index);
}

[[noreturn]]
void sos() {
    while (true) {
        const int c = 100;
        printf("sos\n");
        gpio_put(PICO_DEFAULT_LED_PIN, true);
        sleep_ms(c);
        gpio_put(PICO_DEFAULT_LED_PIN, false);
        sleep_ms(c);
        gpio_put(PICO_DEFAULT_LED_PIN, true);
        sleep_ms(c);
        gpio_put(PICO_DEFAULT_LED_PIN, false);
        sleep_ms(c);
        gpio_put(PICO_DEFAULT_LED_PIN, true);
        sleep_ms(c);
        gpio_put(PICO_DEFAULT_LED_PIN, false);
        sleep_ms(c);

        sleep_ms(c*2);

        gpio_put(PICO_DEFAULT_LED_PIN, true);
        sleep_ms(c*2);
        gpio_put(PICO_DEFAULT_LED_PIN, false);
        sleep_ms(c*2);
        gpio_put(PICO_DEFAULT_LED_PIN, true);
        sleep_ms(c*2);
        gpio_put(PICO_DEFAULT_LED_PIN, false);
        sleep_ms(c*2);
        gpio_put(PICO_DEFAULT_LED_PIN, true);
        sleep_ms(c*2);
        gpio_put(PICO_DEFAULT_LED_PIN, false);
        sleep_ms(c*2);

        sleep_ms(c*2);

        gpio_put(PICO_DEFAULT_LED_PIN, true);
        sleep_ms(c);
        gpio_put(PICO_DEFAULT_LED_PIN, false);
        sleep_ms(c);
        gpio_put(PICO_DEFAULT_LED_PIN, true);
        sleep_ms(c);
        gpio_put(PICO_DEFAULT_LED_PIN, false);
        sleep_ms(c);
        gpio_put(PICO_DEFAULT_LED_PIN, true);
        sleep_ms(c);
        gpio_put(PICO_DEFAULT_LED_PIN, false);
        sleep_ms(c);

        sleep_ms(c*4);
    }
}