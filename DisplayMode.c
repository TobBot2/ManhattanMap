#include "DisplayMode.h"

#include <math.h>

#include "TrainLine.h"

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

// ******************************************************************************
//                              DISPLAY MODE FUNCS
// ******************************************************************************

Color dsp_mode_solid(PixelHandle px, void* data_mode) {
    DisplayData* data = (DisplayData*)data_mode;
    return data->color;
}

Color dsp_mode_train_line_colors(PixelHandle px, void* data_mode) {
    DisplayData* data = (DisplayData*)data_mode;
    return tl_name_to_color(pxtl_search(data->pxtl, px)->name);
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

Color dsp_mode_simulate_trains(PixelHandle px, void* data_mode) {
    DisplayData* data = (DisplayData*)data_mode;

    for (unsigned t = 0; t < data->simulate.count; t++) {
        if (px_compare(px, data->simulate.trains[t].pos) == 0) {
            return color_mix((Color) { 0 }, data->simulate.trains[t].color, data->simulate.trains[t].interp);
        } else if (px_compare(px, data->simulate.trains[t].prev) == 0) {
            return color_mix(data->simulate.trains[t].color, (Color) { 0 }, data->simulate.trains[t].interp);
        }
    }

    return (Color) { 0 };
}

void dsp_mode_simulate_trains_init(void* data_mode) {
    DisplayData* data = (DisplayData*)data_mode;

    for (unsigned t = 0; t < data->simulate.count; t++) {
        float pos_x, pos_y;
        px_get_position(data->simulate.trains[t].pos, &pos_x, &pos_y);
        float prev_x, prev_y;
        px_get_position(data->simulate.trains[t].pos, &prev_x, &prev_y);
        // float b = data->simulate.trains[t].prev;
        // float dist_btwn_px =
    }

}

void dsp_mode_simulate_trains_tick(void* data_mode) {
    DisplayData* data = (DisplayData*)data_mode;

    // for (unsigned t = 0; t < data->simulate.count; t++) {
    //     if (px_compare(px, data->simulate.trains[t].pos) == 0) {
    //         return color_mix((Color) { 0 }, data->simulate.trains[t].color, data->simulate.trains[t].interp);
    //     } else if (px_compare(px, data->simulate.trains[t].prev) == 0) {
    //         return color_mix(data->simulate.trains[t].color, (Color) { 0 }, data->simulate.trains[t].interp);
    //     }
    // }

}