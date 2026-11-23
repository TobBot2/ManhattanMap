#ifndef MANHATTANMAP_DISPLAYMODE_H
#define MANHATTANMAP_DISPLAYMODE_H

#include "Map.h"
#include "Pixel.h"
#include "TrainLine.h"

typedef struct SimulatedTrain_t {
    PixelIndex pos;
    PixelIndex prev;
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

        struct {                            // DISPLAY_MODE_TRAIN_LINE_COLORS
            PixelTrainLines* pxtl;
            TrainLine* tls;
        } tl_colors;

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
void dsp_init_presets(const Map* map);
void dsp_update_map(const Map* map, const DisplayData* data, int64_t frame_time_us);

#endif //MANHATTANMAP_DISPLAYMODE_H
