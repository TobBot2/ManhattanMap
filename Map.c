#include "Map.h"

#include <stdio.h>

#include "config.h"
#include "DisplayMode.h"
#include "quickpix.h"
#include "TrainLine.h"

static PixelGroup pixel_group;

static TrainLine* train_lines;
static unsigned train_lines_cnt;
static PixelTrainLines pixel_train_lines;

static DisplayModeIndex display_mode_index;

void map_init(const Quickpix* quickpix) {
    pxtl_init(&pixel_train_lines, &pixel_group);

    // pixels
    pxg_init(&pixel_group, quickpix);
    for (unsigned s = 0; s < CONFIG.pixel_segments_count; s++) {
        const PixelSegmentConfig* seg = &CONFIG.pixel_segments[s];
        float x = seg->begin_x;
        float y = seg->begin_y;
        float dx = (seg->end_x - seg->begin_x) / (seg->end - seg->begin);
        float dy = (seg->end_y - seg->begin_y) / (seg->end - seg->begin);
        for (PixelIndex p = 0; p <= seg->end; p++) {  // inclusive with end
            px_set_position(&pixel_group, p, x, y);
            x += dx;
            y += dy;
        }
    }

    // train lines
    train_lines = calloc(CONFIG.trainlines_count, sizeof(TrainLine));
    for (unsigned i = 0; i < CONFIG.trainlines_count; i++) {
        const TrainLineConfig* tl_config = &CONFIG.trainline_configs[i];
        Color color = color_from(tl_config->r, tl_config->g, tl_config->b);
        tl_init(&train_lines[i], color);
        const TrainLineSegmentConfig* seg_end = tl_config->segments + CONFIG.trainlines_count;
        for (const TrainLineSegmentConfig* seg = tl_config->segments; seg != seg_end; seg++) {
            tl_add_segment(i, seg->begin, seg->end, train_lines, &pixel_train_lines);
        }
    }
}

void map_update(int64_t frame_time_us) {
    const DisplayData* data = dsp_get_preset(display_mode_index);
    switch (data->mode) {
        case DISPLAY_MODE_SOLID: {
            pxg_apply(&pixel_group, dsp_mode_solid, (void*)data);
            break;
        }
        case DISPLAY_MODE_TRAIN_LINE_COLORS: {
            pxg_apply(&pixel_group, dsp_mode_train_line_colors, (void*)data);
            break;
        }
        case DISPLAY_MODE_WAVE: {
            pxg_apply(&pixel_group, dsp_mode_wave, (void*)data);
            dsp_mode_wave_tick((void*)data, frame_time_us);
            break;
        }
        case DISPLAY_MODE_SIMULATE_TRAINS: {
            pxg_apply(&pixel_group, dsp_mode_simulate_trains, (void*)data);
            dsp_mode_simulate_trains_tick(&pixel_group, (void*)data, frame_time_us);
            break;
        }
        case DISPLAY_MODE_PULSE: {
            pxg_apply(&pixel_group, dsp_mode_pulse, (void*)data);
            dsp_mode_pulse_tick((void*)data, frame_time_us);
            break;
        }
    }
}

void map_clear() {
    for (PixelIndex px = 0; px < pixel_group.count; px++) {
        pixel_group.colors[px] = (Color){ 0 };
    }
}

void map_next_display_mode() {
    dsp_next_preset(&display_mode_index);
}

const PixelGroup* map_get_pxg() {
    return &pixel_group;
}

const PixelTrainLines* map_get_pxtl_map() {
    return &pixel_train_lines;
}

void map_get_train_lines(const TrainLine** tl_out, unsigned* tl_cnt) {
    *tl_out = train_lines;
    *tl_cnt = train_lines_cnt;
}
