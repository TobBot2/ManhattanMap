#include "Pixel.h"

uint32_t color_to_hardware_format(Color c) {
    // store as grb (Color is stored as grb in memory already)
    return (*(uint32_t*)&c) >> 8; // remove trailing garbage data (Color is 24 bits)
}

// ******************************************************************************
//                                 PIXEL GROUP
// ******************************************************************************

void pxg_init(PixelGroup* pxg) {
    pxg->chains = NULL;
    pxg->count = 0;
}

PixelChain* pxg_add_chain(PixelGroup* pxg, unsigned count) {
    // resize
    pxg->count++;
    pxg->chains = realloc(pxg->chains, pxg->count * sizeof(PixelChain));

    // reset new chain
    pxg->chains[pxg->count - 1].data = malloc(count * sizeof(PixelData));
    pxg->chains[pxg->count - 1].count = count;

    return &pxg->chains[pxg->count - 1];
}

void pxg_apply(const PixelGroup* pxg, PixelTransformFn fn, void* aux) {
    PixelHandle cur_px;
    cur_px.group = pxg;
    for (cur_px.chain = 0; cur_px.chain < pxg->count; cur_px.chain++) {
        for (cur_px.index = 0; cur_px.index < pxg->chains[cur_px.chain].count; cur_px.index++) {
            pxg->chains[cur_px.chain].data[cur_px.index].color = fn(cur_px, aux);
        }
    }
}

// ******************************************************************************
//                                   PIXELS
// ******************************************************************************

PixelHandle px_create_empty() {
    return (PixelHandle){
        .index = 0,
        .chain = 0,
        .group = NULL,
    };
}

int px_is_empty(PixelHandle px) {
    return !px.group;
}

int px_compare(PixelHandle lhs, PixelHandle rhs) {
    // chain is more significant than index
    if (lhs.chain == rhs.chain) {
        return (int)lhs.index - (int)rhs.index;
    } else {
        return (int)lhs.chain - (int)rhs.chain;
    }
}

PixelData* px_get_data(PixelHandle px) {
    return px.group->chains[px.chain].data;
}

void px_set_color(PixelHandle px, Color color) {
    px.group->chains[px.chain].data[px.index].color = color;
}
Color px_get_color(PixelHandle px) {
    return px.group->chains[px.chain].data[px.index].color;
}

void px_set_position(PixelHandle px, float x, float y) {
    px.group->chains[px.chain].data[px.index].x = x;
    px.group->chains[px.chain].data[px.index].y = y;
}

void px_set_strip_positions(PixelHandle px, unsigned count, const float p1[2], const float p2[2]) {
    float dx = (p2[0] - p1[0]) / count;
    float dy = (p2[1] - p1[1]) / count;
    for (unsigned i = 0; i <= count; i++) { // inclusive loop [0, count]
        px.group->chains[px.chain].data[px.index].x = p1[0] + dx * i;
        px.group->chains[px.chain].data[px.index].y = p1[1] + dy * i;
    }
}
void px_get_position(PixelHandle px, float* out_x, float* out_y) {
    *out_x = px.group->chains[px.chain].data[px.index].x;
    *out_y = px.group->chains[px.chain].data[px.index].y;
}

void px_next(PixelHandle* px) {
    px->index++;

    // wrap around index
    if (px->index >= px->group->chains[px->chain].count) {
        px->index = 0;
        px->chain++;

        // wrap around chain
        if (px->chain >= px->group->count) {
            px->chain = 0;
        }
    }
}
