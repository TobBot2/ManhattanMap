#include <stdio.h>

#include "hardware/gpio.h"
#include "pico/stdio.h"
#include "ws2812.pio.h"
#include "pico/time.h"

#include "PixelChain.h"
#include "TrainLine.h"

#define IS_RGBW false; // RGB vs RBGW

#define WS2812_PIN 6;
#define BUTTON_PIN 7;

#define DEBUG_LIGHT_NUMBERS true;

// callbacks
static void debug_next_light_cb(uint gpio, uint32_t events);
static void next_display_mode_cb(uint gpio, uint32_t events);

// init stuff
static void initialize();
static void setup_lights();
static void setup_train_lines();

static void show();
static void sos();

// GLOBAL VARIABLES

static PixelHandle debugLight = {
    .chain = 0,
    .index = 0,
};
static uint32_t debugColor = 0xff0000;

static PIOHandle pioHandle;
static PixelChain[2] pixelsChains;

static DisplayMode mode = Solid;
static DisplayData data = Color(255, 255, 255);

[[noreturn]]
int main(void) {
    initialize();

    if (DEBUG_LIGHT_NUMBERS) {
        printf("debug mode enabled. initializing.\n");

        gpio_init(BUTTON_PIN);
        gpio_pull_up(BUTTON_PIN);
        gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &debug_next_light_cb);

        printf("debug loop executing.\n");

        while (true) {
            show();
        }
    }

    gpio_init(BUTTON_PIN);
    gpio_pull_up(BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_FALL, true, &next_display_mode_cb);

    std::array<Light, TOTAL_LIGHTS_COUNT> lights;
    std::array<TrainLine, TRAIN_LINES_COUNT> trainLines;

    // setup_lights(lights);
    // setup_train_lines(trainLines);

    while (true) {
        switch (mode) {
            case Solid: {
                for (Light& l : lights) {
                    l.setColor(std::get<Color>(data));
                }
                break;
            }
            case TrainLineColors: {
                for (int i = 0; i < TRAIN_LINES_COUNT; i++) {
                    Color lineColor = getTrainLineColor(static_cast<TrainLineName>(i));
                    for (LightIndex& l : trainLines[i].getLights()) {
                        lights[l].setColor(lineColor);
                    }
                }
                break;
            }
            case Gradient: {
                for (Light& l : lights) {
                    const auto& [color1, color2, pos1, pos2] = std::get<2>(data);

                    float distance = vmath_hpp::distance(pos1, pos2);
                    vmath_hpp::fvec2 direction = (pos2 - pos1) / distance;
                    float percent = vmath_hpp::dot(direction, l.getPosition() - pos1) / distance;

                    Color interpolatedColor = {
                        color1 * (1.f - percent) + color2 * percent
                    };
                    l.setColor(interpolatedColor);
                }
                break;
            }
            case SimulateTrains: {
                // TODO
                break;
            }
            case Pulse:
                break;
        }

        // update pixels to match `Light` objects
        for (int i = 0; i < TOTAL_LIGHTS_COUNT; i++)
            pixels[i] = color_to_grb(lights[i].getColor());

        show();
    }
}

static void debug_next_light_cb(uint gpio, uint32_t events) {
    printf(".");

    // ignore multi-presses
    static uint32_t last_time = 0;
    uint32_t now = to_ms_since_boot(get_absolute_time());
    if (now - last_time < 200) return;
    last_time = now;

    debugLight = (debugLight + 1) % TOTAL_LIGHTS_COUNT;
    printf("\nLight idx: %hu", debugLight);

    pixels[debugLight] = debugColor;

    // alter previous pixels
    for (int i = 1; i <= 1; i++) {
        int idx = debugLight - i;
        while (idx < 0) idx += TOTAL_LIGHTS_COUNT;
        pixels[idx] = 0;
    }
}

static void next_display_mode_cb(uint gpio, uint32_t events) {
    switch (mode) {
        case Solid: {
            switch (color_to_grb(std::get<Color>(data))) {
                // cycle through more colors...
                case (color_to_grb({255, 255, 255})):
                    data = Color{ 255, 0, 255 }; // TODO: use fixed array for colors (defined similar to enum)? Then have data store an index instead of color
                    break;

                default: // once done cycling through colors, next mode
                    mode = TrainLineColors;
                    data = std::monostate{};
                    break;
            }
            break;
        }

        case TrainLineColors:
            mode = Gradient;
            // data = { Color{ 255, 255, 0 }, Color{ 0, 255, 255 }, vmath_hpp::fvec2{ 7.f, 2.f }, vmath_hpp::fvec2{ 1.f, 1.f } };
            break;
        case Gradient:
            break;
        case SimulateTrains:
            break;
        case Pulse:
            break;
    }
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

    if (DEBUG_LIGHT_NUMBERS) {
        sleep_ms(5000); // give extra time to connect to port
        printf("Initialized stdio.\n");
    } else {
        sleep_ms(2000);
    }

    if (!pio_claim_free_sm_and_add_program_for_gpio_range(&ws2812_program, &pioHandle.pio, &pioHandle.sm, &pioHandle.offset, WS2812_PIN, 1, true)) {
        printf("PIO claim resources failed.\n");
        sos();
    }

    ws2812_program_init(pioHandle.pio, pioHandle.sm, pioHandle.offset, WS2812_PIN, 800000, IS_RGBW);
}

static void setup_lights(std::array<Light, TOTAL_LIGHTS_COUNT>& lights) {
    // TODO

    // register all lights
    setStripPositions(lights, 0, 60, {7.f, .2f}, {7.f, 24.f});

    // intersections
    lights[12].setIsIntersection();
}

static void setup_train_lines(std::array<TrainLine, TRAIN_LINES_COUNT>& trainLines) {
    trainLines[LINE_1].setLights(TrainLine::generateTrainLineLights({
        {0, 66}, {84, 101}
    }));

}

static void show() {
    if (!pioHandle.pio) {
        printf("PIO not initialized before writing pixels");
        sos();
    }

    for (int i = 0; i < TOTAL_LIGHTS_COUNT; i++)
        pio_sm_put_blocking(pioHandle.pio, pioHandle.sm, pixels[i] << 8u); // shift so color is in upper 24 bits

    sleep_ms(10); // ensure protocol latch (minimum 50 us)
}

void sos() {
    int c = 100;
    while (true) {
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