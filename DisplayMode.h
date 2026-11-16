#ifndef MANHATTANMAP_DISPLAYMODE_H
#define MANHATTANMAP_DISPLAYMODE_H

#include "Pixel.h"
#include "TrainLine.h"

typedef struct SimulatedTrain_t {
    PixelHandle pos;
    PixelHandle prev;
    float interp; // interpolation between prev and curr position.
    Color color;
} SimulatedTrain;

typedef unsigned DisplayModeIndex;

typedef enum DisplayMode_t {
    DISPLAY_MODE_SOLID,                     // all solid colors
    DISPLAY_MODE_TRAIN_LINE_COLORS,         // train lines colored solid of official mta line colors
    DISPLAY_MODE_WAVE,                      // linear colors blend between two colored points
    DISPLAY_MODE_PULSE,                     // pulse from a point
    DISPLAY_MODE_SIMULATE_TRAINS,           // simulate trains
} DisplayMode;

typedef struct DisplayData_t {
    DisplayMode mode;

    union {
        Color color;                        // DISPLAY_MODE_SOLID

        PixelTrainLineMap* pxtl;            // DISPLAY_MODE_TRAIN_LINE_COLORS

        struct {                            // DISPLAY_MODE_WAVE
            Color color1;
            Color color2;
            float dir_x, dir_y;
            float speed; // in/s
            uint64_t elapsed_ms;
        } wave;

        struct {                            // DISPLAY_MODE_PULSE
            Color color;
            float x, y;
            float speed; // in/s
            uint64_t elapsed_ms;
        } pulse;

        struct {                            // DISPLAY_MODE_SIMULATE_TRAINS
            int count;
            float speed; // in/s
            SimulatedTrain* trains;
        } simulate;
    };
} DisplayData;

// DISPLAY DATA

void dsp_next_preset(DisplayModeIndex* idx); // modifies to be next index (wraps)
const DisplayData* dsp_get_preset(DisplayModeIndex idx); // returns ptr to static const arr
void dsp_init_presets();

// DISPLAY MODE FUNCTIONS

Color dsp_mode_solid(PixelHandle px, void* data_mode);
Color dsp_mode_train_line_colors(PixelHandle px, void* data_mode);
Color dsp_mode_wave(PixelHandle px, void* data_mode);
void dsp_mode_wave_tick(void* data_mode, int64_t elapsed_us);
Color dsp_mode_pulse(PixelHandle px, void* data_mode);
void dsp_mode_pulse_tick(void* data_mode, int64_t elapsed_us);
Color dsp_mode_simulate_trains(PixelHandle px, void* data_mode);
void dsp_mode_simulate_trains_init(void* data_mode);
void dsp_mode_simulate_trains_tick(void* data_mode, int64_t elapsed_us);

#endif //MANHATTANMAP_DISPLAYMODE_H
