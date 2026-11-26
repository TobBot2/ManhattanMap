#include "DisplayMode.h"

#include <math.h>

// array of all display presets, accessible via index
static DisplayData DISPLAY_MODE_PRESETS[] = {
    (DisplayData){
        .mode = DISPLAY_MODE_SOLID,
        .color = color_from_rgb(0x00, 0x80, 0xff)
    },
    (DisplayData){
        .mode = DISPLAY_MODE_TRAIN_LINE_COLORS,
    },
    (DisplayData){
        .mode = DISPLAY_MODE_WAVE,
        .wave = {
            .color1 = color_from_rgb(0xff, 0xff, 0xff),
            .color2 = color_from_rgb(0x00, 0x00, 0x00),
            .dir_x = 0, .dir_y = -1, // south
            .speed = .0018f,
            .elapsed_ms = 0.f
        }
    },
    (DisplayData){
        .mode = DISPLAY_MODE_PULSE,
        .pulse = {
            .color = color_from_rgb(0xff, 0xff, 0xff),
            .x = 0, .y = 0,
            .speed = .0018f,
            .elapsed_ms = 0.f
        }
    },
    (DisplayData){
        .mode = DISPLAY_MODE_SIMULATE_TRAINS,
        .simulate = {
            .count = 8,
            .speed = .00002f,
            .trains = (SimulatedTrain[8]) { 0 }
        }
    }
};

static const DisplayModeIndex DISPLAY_MODE_PRESETS_CNT = sizeof(DISPLAY_MODE_PRESETS) / sizeof(DisplayData);

static void dsp_mode_solid(const PixelGroup* pxg, PixelIndex px, void* data_mode);
static void dsp_mode_train_line_colors(const PixelGroup* pxg, PixelIndex px, void* data_mode);
static void dsp_mode_train_line_colors_init(const Map* map, void* data_mode);
static void dsp_mode_wave(const PixelGroup* pxg, PixelIndex px, void* data_mode);
static void dsp_mode_wave_tick(void* data_mode, int64_t elapsed_us);
static void dsp_mode_pulse(const PixelGroup* pxg, PixelIndex px, void* data_mode);
static void dsp_mode_pulse_tick(void* data_mode, int64_t elapsed_us);
static void dsp_mode_simulate_trains(const PixelGroup* pxg, PixelIndex px, void* data_mode);
static void dsp_mode_simulate_trains_init(const Map* map, void* data_mode);
static void dsp_mode_simulate_trains_tick(const Map* map, void* data_mode, int64_t elapsed_us);

static float dist2d(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return sqrtf(dx * dx + dy * dy);
}

void dsp_next_preset(DisplayModeIndex* idx) {
    *idx = (*idx + 1) % DISPLAY_MODE_PRESETS_CNT;
}

const DisplayData* dsp_get_preset(DisplayModeIndex idx) {
    return &DISPLAY_MODE_PRESETS[idx];
}

void dsp_init_presets(const Map* map) {
    for (unsigned i = 0; i < DISPLAY_MODE_PRESETS_CNT; i++) {
        DisplayData* data = (DisplayData*)&DISPLAY_MODE_PRESETS[i];
        switch (data->mode) {
            case DISPLAY_MODE_TRAIN_LINE_COLORS: {
                dsp_mode_train_line_colors_init(map, data);
                break;
            }
            case DISPLAY_MODE_SIMULATE_TRAINS: {
                dsp_mode_simulate_trains_init(map, data);
                break;
            }
            default:
                break; // most display modes don't need initialization...
        }
    }
}

void dsp_update_map(const Map* map, const DisplayData* data, int64_t frame_time_us) {
    switch (data->mode) {
        case DISPLAY_MODE_SOLID: {
            pxg_apply(&map->pxg, dsp_mode_solid, (void*)data);
            break;
        }
        case DISPLAY_MODE_TRAIN_LINE_COLORS: {
            pxg_apply(&map->pxg, dsp_mode_train_line_colors, (void*)data);
            break;
        }
        case DISPLAY_MODE_WAVE: {
            pxg_apply(&map->pxg, dsp_mode_wave, (void*)data);
            dsp_mode_wave_tick((void*)data, frame_time_us);
            break;
        }
        case DISPLAY_MODE_SIMULATE_TRAINS: {
            pxg_apply(&map->pxg, dsp_mode_simulate_trains, (void*)data);
            dsp_mode_simulate_trains_tick(map, (void*)data, frame_time_us);
            break;
        }
        case DISPLAY_MODE_PULSE: {
            pxg_apply(&map->pxg, dsp_mode_pulse, (void*)data);
            dsp_mode_pulse_tick((void*)data, frame_time_us);
            break;
        }
    }
}

// ******************************************************************************
//                              DISPLAY MODE FUNCS
// ******************************************************************************

static void dsp_mode_solid(const PixelGroup* pxg, PixelIndex px, void* data_mode) {
    DisplayData* data = (DisplayData*)data_mode;
    px_set_color(pxg, px, data->color);
}

static void dsp_mode_train_line_colors(const PixelGroup* pxg, PixelIndex px, void* data_mode) {
    DisplayData* data = (DisplayData*)data_mode;
    unsigned r = 0, g = 0, b = 0;
    unsigned count = 0;
    for (uint32_t m = data->tl_colors.pxtl->tl_masks[px]; m; m &= m - 1) {
        const TrainLine* tl = &data->tl_colors.tls[__builtin_ctz(m)]; // mask to tl_index to tl
        r += tl->color.r;
        g += tl->color.g;
        b += tl->color.b;
        count++;
    }

    if (count == 0) {
        px_set_color(pxg, px, color_from_rgb(0, 0, 0));
        return;
    }

    px_set_color(pxg, px, color_from_rgb(r / count, g / count, b / count));
}

static void dsp_mode_train_line_colors_init(const Map* map, void* data_mode) {
    DisplayData* data = (DisplayData*)data_mode;
    data->tl_colors.pxtl = &map->pxtl;
    data->tl_colors.tls = map->tls;

    printf("dsp:\n");
    printf("map->pxtl = %p\n", &map->pxtl);
    printf("data->pxtl = %p\n", dsp_get_preset(1)->tl_colors.pxtl);

    printf("map->tls = %p\n", &map->tls);
    printf("data->tls = %p\n", dsp_get_preset(1)->tl_colors.tls);

}

static void dsp_mode_wave(const PixelGroup* pxg, PixelIndex px, void* data_mode) {
    DisplayData* data = (DisplayData*)data_mode;

    const float wave_length = 10.f;

    // get distance along direction of wave
    float px_x, px_y;
    px_get_position(pxg, px, &px_x, &px_y);
    float distance = px_x * data->wave.dir_x + px_y + data->wave.dir_y; // dot product

    // get mix between color1 and color2
    float offset = data->wave.elapsed_ms * data->wave.speed;
    float mix = (distance + offset) / wave_length; // normalize
    mix -= floorf(mix); // clamp [0, 1)
    if (mix >= 1.f) mix = 0.f;

    px_set_color(pxg, px, color_mix(data->wave.color1, data->wave.color2, mix));
}

static void dsp_mode_wave_tick(void* data_mode, int64_t elapsed_us) {
    DisplayData* data = (DisplayData*)data_mode;
    data->wave.elapsed_ms += elapsed_us / 1000;
}

static void dsp_mode_pulse(const PixelGroup* pxg, PixelIndex px, void* data_mode) {
    DisplayData* data = (DisplayData*)data_mode;

    const float pulse_length = 10.f;

    float px_x, px_y;
    px_get_position(pxg, px, &px_x, &px_y);
    float a = px_x - data->pulse.x;
    float b = px_y - data->pulse.y;
    float distance = sqrt(a*a + b*b);

    float offset = data->pulse.elapsed_ms * data->pulse.speed;
    float mix = (distance + offset) / pulse_length; // normalize
    mix -= floorf(mix); // clamp [0, 1)
    if (mix >= 1.f) mix = 0.f;

    px_set_color(pxg, px, color_mix(data->pulse.color, (Color){ 0, 0, 0, 0 }, mix));
}

static void dsp_mode_pulse_tick(void* data_mode, int64_t elapsed_us) {
    DisplayData* data = (DisplayData*)data_mode;
    data->pulse.elapsed_ms += elapsed_us / 1000;
}

static void dsp_mode_simulate_trains(const PixelGroup* pxg, PixelIndex px, void* data_mode) {
    DisplayData* data = (DisplayData*)data_mode;

    unsigned r = 0, g = 0, b = 0;
    unsigned count = 0;

    for (unsigned t = 0; t < data->simulate.count; t++) {
        if (px != data->simulate.trains[t].curr && px != data->simulate.trains[t].prev) {
            continue;
        }

        Color c;
        if (px == data->simulate.trains[t].curr) {
            c = color_mix(color_from_rgb(0, 0, 0), data->simulate.trains[t].color, data->simulate.trains[t].interp);
        } else {
            c = color_mix(data->simulate.trains[t].color, color_from_rgb(0, 0, 0), data->simulate.trains[t].interp);
        }
        r += c.r;
        g += c.g;
        b += c.b;
        count++;
    }

    if (count == 0) {
        px_set_color(pxg, px, color_from_rgb(0, 0, 0));
    } else {
        px_set_color(pxg, px, color_from_rgb(r / count, g / count, b / count));
    }

}

SimulatedTrain dsp_mode_simulate_trains_begin_random_train(const Map* map) {
    // get random train line
    const TrainLine* tl = &map->tls[rand() % map->tls_count];

    // begin on a random end of train line
    PixelIndex begin_px;
    if (rand() % 2 == 0) {
        begin_px = tl->pixels[tl->pixels_count - 1];
    } else {
        begin_px = tl->pixels[0];
    }

    return (SimulatedTrain) {
        .interp = 0.f,
        .curr = begin_px,
        .prev = begin_px, // prev = pos -> can go in either dir (but only one valid dir exists)
        .color = color_from_rgb(255, 255, 255)
    };
}

static void dsp_mode_simulate_trains_init(const Map* map, void* data_mode) {
    DisplayData* data = (DisplayData*)data_mode;

    for (unsigned t = 0; t < data->simulate.count; t++) {
        data->simulate.trains[t] = dsp_mode_simulate_trains_begin_random_train(map);
    }

}

static void dsp_mode_simulate_trains_tick(const Map* map, void* data_mode, int64_t elapsed_us) {
    DisplayData* data = (DisplayData*)data_mode;

    float px1_x, px1_y;
    float px2_x, px2_y;

    for (unsigned t = 0; t < data->simulate.count; t++) {
        SimulatedTrain* train = &data->simulate.trains[t];

        px_get_position(&map->pxg, train->curr, &px1_x, &px1_y);
        px_get_position(&map->pxg, train->prev, &px2_x, &px2_y);
        float dist = dist2d(px1_x, px1_y, px2_x, px2_y);
        train->interp += data->simulate.speed * elapsed_us / 1000000 / dist;

        if (train->interp >= 1.f) {
            float overshoot = (1 - train->interp) * dist;

            PixelIndex next = map_get_random_next_neighbor(map, train->curr, train->prev);
            if (next < 0) { // no next found... restart train
                *train = dsp_mode_simulate_trains_begin_random_train(map);
            }

            // calc new distance
            px_get_position(&map->pxg, train->curr, &px1_x, &px1_y);
            px_get_position(&map->pxg, train->prev, &px2_x, &px2_y);
            dist = dist2d(px1_x, px1_y, px2_x, px2_y);

            train->prev = train->curr;
            train->curr = next;
            train->interp = overshoot / dist;
        }
    }

}