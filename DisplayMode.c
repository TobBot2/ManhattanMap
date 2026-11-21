#include "DisplayMode.h"

#include <math.h>

#include "TrainLine.h"
#include "Map.h"

// array of all display presets, accessible via index
static const DisplayData DISPLAY_MODE_PRESETS[] = {
    (DisplayData){
        .mode = DISPLAY_MODE_SOLID,
        .color = (Color){ 0xff, 0xff, 0xff }
    },
    (DisplayData){
        .mode = DISPLAY_MODE_TRAIN_LINE_COLORS,
    },
    (DisplayData){
        .mode = DISPLAY_MODE_WAVE,
        .wave = {
            .color1 = (Color) { 0xff, 0xff, 0xff },
            .color2 = (Color) { 0x00, 0x00, 0x00 },
            .dir_x = 0, .dir_y = -1, // south
            .speed = 1.8f
        }
    },
    (DisplayData){
        .mode = DISPLAY_MODE_PULSE,
        .pulse = {
            .color = (Color) { 0xff, 0xff, 0xff },
            .x = 0, .y = 0,
            .speed = 1.8f
        }
    },
    (DisplayData){
        .mode = DISPLAY_MODE_SIMULATE_TRAINS,
        .simulate = {
            .count = 8,
            .speed = .02f,
            .trains = (SimulatedTrain[8]){}
        }
    }
};

static const DisplayModeIndex DISPLAY_MODE_PRESETS_CNT = sizeof(DISPLAY_MODE_PRESETS) / sizeof(DisplayData);

void dsp_next_preset(DisplayModeIndex* idx) {
    *idx = (*idx + 1) % DISPLAY_MODE_PRESETS_CNT;
}

const DisplayData* dsp_get_preset(DisplayModeIndex idx) {
    return &DISPLAY_MODE_PRESETS[idx];
}

void dsp_init_presets() {
    for (unsigned i = 0; i < DISPLAY_MODE_PRESETS_CNT; i++) {
        DisplayData data = (DisplayData)DISPLAY_MODE_PRESETS[i];
        switch (data.mode) {
            case DISPLAY_MODE_SOLID:
            case DISPLAY_MODE_TRAIN_LINE_COLORS:
            case DISPLAY_MODE_PULSE:
            case DISPLAY_MODE_WAVE:
                break; // most don't need any further initialization than the map being ready...
            case DISPLAY_MODE_SIMULATE_TRAINS: {
                dsp_mode_simulate_trains_init(&data);
            }
        }
    }
}

// ******************************************************************************
//                              DISPLAY MODE FUNCS
// ******************************************************************************

Color dsp_mode_solid(PixelHandle px, void* data_mode) {
    DisplayData* data = (DisplayData*)data_mode;
    return data->color;
}

Color dsp_mode_train_line_colors(PixelHandle px, void* data_mode) {
    DisplayData* data = (DisplayData*)data_mode;
    return pxtl_map_search(data->pxtl_map, px)->lines[0]->color;
}

Color dsp_mode_wave(PixelHandle px, void* data_mode) {
    DisplayData* data = (DisplayData*)data_mode;

    const float wave_length = 10.f;

    // get distance along direction of wave
    float px_x, px_y;
    px_get_position(px, &px_x, &px_y);
    float distance = px_x * data->wave.dir_x + px_y + data->wave.dir_y; // dot product

    // get mix between color1 and color2
    float offset = data->wave.elapsed_ms / 1000.f * data->wave.speed;
    float mix = (distance + offset) / wave_length; // normalize
    while (mix >= 1.f) mix -= 1.f; // clamp [0, 1)
    while (mix < 0.f)  mix += 1.f;
    if (mix > .5f) mix = 1.f - mix;
    mix *= 2.f;

    return color_mix(data->wave.color1, data->wave.color2, mix);
}

void dsp_mode_wave_tick(void* data_mode, int64_t elapsed_us) {
    DisplayData* data = (DisplayData*)data_mode;
    data->wave.elapsed_ms += elapsed_us / 1000;
}

Color dsp_mode_pulse(PixelHandle px, void* data_mode) {
    DisplayData* data = (DisplayData*)data_mode;

    const float pulse_length = 10.f;

    float px_x, px_y;
    px_get_position(px, &px_x, &px_y);
    float a = px_x - data->pulse.x;
    float b = px_y - data->pulse.y;
    float distance = sqrt(a*a + b*b);

    float offset = data->pulse.elapsed_ms / 1000.f * data->pulse.speed;
    float mix = (distance + offset) / pulse_length; // normalize
    while (mix >= 1.f) mix -= 1.f;
    while (mix < 0.f)  mix += 1.f;
    if (mix > .5f) mix = 1.f - mix;
    mix *= 2.f;

    return color_mix(data->pulse.color, (Color){ 0, 0, 0, 0 }, mix);
}

void dsp_mode_pulse_tick(void* data_mode, int64_t elapsed_us) {
    DisplayData* data = (DisplayData*)data_mode;
    data->pulse.elapsed_ms += elapsed_us / 1000;
}

Color dsp_mode_simulate_trains(PixelHandle px, void* data_mode) {
    DisplayData* data = (DisplayData*)data_mode;

    for (unsigned t = 0; t < data->simulate.count; t++) {
        if (px_compare(px, data->simulate.trains[t].pos) == 0) {
            return color_mix((Color) { 0 }, data->simulate.trains[t].color, data->simulate.trains[t].interp);
        } else if (px_compare(px, data->simulate.trains[t].prev) == 0) {
            return color_mix(data->simulate.trains[t].color, (Color) { 0 }, data->simulate.trains[t].interp);
        }
    }

    return color_from(0x10, 0x10, 0x10);
}

SimulatedTrain dsp_mode_simulate_trains_begin_random_train() {
    // get random train line
    const TrainLine* tls = NULL;
    unsigned tls_cnt;
    map_get_train_lines(&tls, &tls_cnt);
    const TrainLine* tl = &tls[rand() % tls_cnt];

    // begin on a random end of train line
    PixelHandle begin_px;
    if (rand() % 2 == 0) {
        begin_px = tl->pixels[tl->pixels_cnt - 1];
    } else {
        begin_px = tl->pixels[0];
    }

    return (SimulatedTrain) {
        .interp = 0.f,
        .pos = begin_px,
        .prev = begin_px, // prev = pos -> can go in either dir (but only one valid dir exists)
        .color = color_from(255, 255, 255)
    };
}

void dsp_mode_simulate_trains_init(void* data_mode) {
    DisplayData* data = (DisplayData*)data_mode;

    for (unsigned t = 0; t < data->simulate.count; t++) {
        data->simulate.trains[t] = dsp_mode_simulate_trains_begin_random_train();
    }

}

void dsp_mode_simulate_trains_tick(void* data_mode, int64_t elapsed_us) {
    DisplayData* data = (DisplayData*)data_mode;

    for (unsigned t = 0; t < data->simulate.count; t++) {
        SimulatedTrain* train = &data->simulate.trains[t];

        float dist = px_dist(train->pos, train->prev);
        train->interp += data->simulate.speed * elapsed_us / 1000000;

        if (train->interp >= 1.f) {
            float overshoot = (1 - train->interp) * dist;

            PixelHandle next = tl_get_random_next_neighbor(train->pos, train->prev, map_get_pxtl_map());
            if (px_is_empty(next)) { // no next found... restart train
                *train = dsp_mode_simulate_trains_begin_random_train();
            }

            train->prev = train->pos;
            train->pos = next;
            train->interp = overshoot / px_dist(train->pos, train->prev);
        }
    }

}