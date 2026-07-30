#include <stdio.h>

#include "hardware/gpio.h"
#include "pico/stdio.h"
#include "ws2812.pio.h"
#include "pico/time.h"
#include "hardware/pio.h"
#include "pico/types.h"

#include "quickpix.h"
#include "Pixel.h"
#include "TrainLine.h"
#include "DisplayMode.h"

#define BUTTON_PIN 3

#define DEBUG (false)

// callbacks
static void debug_next_light_cb(uint pin, uint32_t events);
static void next_display_mode_cb(uint pin, uint32_t events);

// init stuff
static void initialize();
static void setup_lights();
static void setup_map();

static void show();
static void sos();

// GLOBAL VARIABLES

#ifdef DEBUG
static PixelHandle debugLight = {
    .chain = 0,
    .index = 0,
};
#endif

static QuickpixPio pio_handle;

static PixelGroup pixel_group;
static DisplayModeIndex display_mode_index;

[[noreturn]]
int main(void) {
    initialize();

#ifdef DEBUG
    printf("debug mode enabled. initializing.\n");

    gpio_init(BUTTON_PIN);
    gpio_pull_up(BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &debug_next_light_cb);

    printf("debug loop executing.\n");

    while (true) {
        show();
    }
#endif

    gpio_init(BUTTON_PIN);
    gpio_pull_up(BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &next_display_mode_cb);

    setup_lights();
    setup_map();

    while (true) {
        const DisplayData* data = dsp_get_preset(display_mode_index);
        switch (data->mode) {
            case DISPLAY_MODE_SOLID: {
                // TODO pxg_apply(&pixel_group, );
                break;
            }
            case DISPLAY_MODE_TRAIN_LINE_COLORS: {
                break;
            }
            case DISPLAY_MODE_WAVE: {
                break;
            }
            case DISPLAY_MODE_SIMULATE_TRAINS: {
                break;
            }
            case DISPLAY_MODE_PULSE:
                break;
        }

        // update pixels to match `Light` objects
        pxg_write(&pixel_group);
    }
}

#ifdef DEBUG
static void debug_next_light_cb(uint pin, uint32_t events) {
    printf(".");

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
#endif

static void next_display_mode_cb(uint pin, uint32_t events) {
    dsp_next_preset(&display_mode_index);
}

static void initialize() {
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    // init sequence
    for (int i = 0; i < 10; i++) {
        gpio_put(PICO_DEFAULT_LED_PIN, true);
        sleep_ms(50);
        gpio_put(PICO_DEFAULT_LED_PIN, false);
        sleep_ms(50);
    }

    stdio_init_all();

    sleep_ms(2000);

#ifdef DEBUG
    sleep_ms(3000); // give extra time to connect to port
    printf("Initialized stdio.\n");
#endif

    if (!pio_claim_free_sm_and_add_program_for_gpio_range(&ws2812_program, &pio_handle.pio, &pio_handle.sm, &pio_handle.offset, WS2812_PIN, WS2812_COUNT, true)) {
        printf("PIO claim resources failed.\n");
        sos();
    }

    ws2812_program_init(pio_handle.pio, pio_handle.sm, pio_handle.offset, WS2812_PIN, 800000, IS_RGBW);
}

static void setup_lights() {
    // TODO
    pxg_init(&pixel_group);

    // register physical pixel chains
    int chain_pin = WS2812_PIN;
    PixelChain* chain1 = pxg_add_chain(&pixel_group, chain_pin++);
    PixelChain* chain2 = pxg_add_chain(&pixel_group, chain_pin++);
    assert(chain_pin == WS2812_PIN + WS2812_COUNT); // create the correct number of chains...

    // register lights (index + location)
    setStripPositions(lights, 0, 60, {7.f, .2f}, {7.f, 24.f});
}

static void setup_map() {
    // intersections

    // train lines
    trainLines[LINE_1].setLights(TrainLine::generateTrainLineLights({
        {0, 66}, {84, 101}
    }));

}

static void show() {
    if (!pio_handle.pio) {
        printf("PIO not initialized before writing pixels");
        sos();
    }

    for (int i = 0; i < TOTAL_LIGHTS_COUNT; i++)
        pio_sm_put_blocking(pio_handle.pio, pio_handle.sm, pixels[i] << 8u); // shift so color is in upper 24 bits

    sleep_ms(10); // ensure protocol latch (minimum 50 us)
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