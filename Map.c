#include "Map.h"

#include <stdio.h>

#include "config.h"
#include "DisplayMode.h"
#include "quickpix.h"
#include "TrainLine.h"

static Quickpix quickpix;

static PixelGroup pixel_group;

static TrainLine* train_lines;
static unsigned train_lines_cnt;
static PixelTrainLinesMap pixel_train_lines_map;

static DisplayModeIndex display_mode_index;

void register_map();

// TODO: should pass in config struct...
void map_init() {
    // quickpix
    quickpix_init(&quickpix);
    for (unsigned i = 0; i < CONFIG.chains_count; i++) {
        const ChainConfig* chain = &CONFIG.chain_configs[i];
        quickpix_add_chain(&quickpix, chain->pin, chain->leds_count);
    }

    register_map();
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
            dsp_mode_simulate_trains_tick((void*)data, frame_time_us);
            break;
        }
        case DISPLAY_MODE_PULSE: {
            pxg_apply(&pixel_group, dsp_mode_pulse, (void*)data);
            dsp_mode_pulse_tick((void*)data, frame_time_us);
            break;
        }
    }
}

void map_display() {
    // update hardware pixel array
    for (unsigned chain_idx = 0; chain_idx < pixel_group.count; chain_idx++) {
        for (PixelIndex px_idx = 0; px_idx < pixel_group.chains[chain_idx].count; px_idx++) {
            Color c = pixel_group.chains[chain_idx].data[px_idx].color;
            c.r *= CONFIG.max_brightness;
            c.g *= CONFIG.max_brightness;
            c.b *= CONFIG.max_brightness;
            quickpix.chains[chain_idx].pixels[px_idx] = color_to_hardware_format(c);
        }
    }

    quickpix_show(&quickpix);
}

void map_clear() {
    for (unsigned chain_idx = 0; chain_idx < pixel_group.count; chain_idx++) {
        for (PixelIndex px_idx = 0; px_idx < pixel_group.chains[chain_idx].count; px_idx++) {
            quickpix.chains[chain_idx].pixels[px_idx] = (uint32_t) 0;
        }
    }

    quickpix_show(&quickpix);
}

void map_next_display_mode() {
    dsp_next_preset(&display_mode_index);
}

const PixelGroup* map_get_pxg() {
    return &pixel_group;
}

const PixelTrainLinesMap* map_get_pxtl_map() {
    return &pixel_train_lines_map;
}

void map_get_train_lines(const TrainLine** tl_out, unsigned* tl_cnt) {
    *tl_out = train_lines;
    *tl_cnt = train_lines_cnt;
}

// HELPER FUNCTIONS

void register_pixels() {

}

void register_map() {
    // init globals
    pxtl_map_init(&pixel_train_lines_map);

    // pixels
    pxg_init(&pixel_group);
    for (unsigned i = 0; i < quickpix.count; i++) {
        pxg_add_chain(&pixel_group, quickpix.chains[i].count);
    }
    for (unsigned s = 0; s < CONFIG.pixel_segments_count; s++) {
        const PixelSegmentConfig* seg = &CONFIG.pixel_segments[s];
        float x = seg->begin_x;
        float y = seg->begin_y;
        float dx = (seg->end_x - seg->begin_x) / px_compare(seg->end, seg->begin);
        float dy = (seg->end_y - seg->begin_y) / px_compare(seg->end, seg->begin);
        for (PixelHandle p = seg->begin; px_compare(seg->end, p) < 0; px_next(&p)) { // inclusive with end
            px_set_position(p, x, y);
            x += dx;
            y += dy;
        }
    }

    // train lines
    train_lines = calloc(CONFIG.trainlines_count, sizeof(TrainLine));
    for (unsigned i = 0; i < CONFIG.trainlines_count; i++) {
        tl_init(&train_lines[i], CONFIG.trainline_configs[i].color);
        const TrainLineSegmentConfig* seg_end = CONFIG.trainline_configs[i].segments + CONFIG.trainlines_count;
        for (const TrainLineSegmentConfig* seg = CONFIG.trainline_configs[i].segments; seg != seg_end; seg++) {
            tl_add_segment(&train_lines[i], seg->begin, seg->end, &pixel_train_lines_map);
        }
    }
}