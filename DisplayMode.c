#include "DisplayMode.h"

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
    tl_name_to_color(pxtl_search(, px));
}

Color dsp_mode_wave(PixelHandle px, void* data_mode) {

}

Color dsp_mode_pulse(PixelHandle px, void* data_mode) {

}

Color dsp_mode_simulate_trains(PixelHandle px, void* data_mode) {

}
