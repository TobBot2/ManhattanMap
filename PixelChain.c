#include "PixelChain.h"

uint32_t color_to_hardware_format(Color c) {
    // store as grb (Color is stored as grb in memory already)
    return (*(uint32_t*)&c) >> 8; // remove trailing garbage data (Color is 24 bits)
}

PixelData* px_get_data(PixelHandle px, const PixelGroup* pixel_group) {
    return pixel_group->chains[px.chain].data;
}

void px_set_intersection(PixelHandle* px, const PixelHandle* neighbors, PixelIndex neighbors_cnt) {
    free(px->neighbors);
    px->neighbors = malloc(neighbors_cnt * sizeof(PixelHandle));
    for (PixelIndex i = 0; i < neighbors_cnt; i++) {
        px->neighbors[i] = neighbors[i];
    }
}

PixelHandle* px_get_neighbors(const PixelHandle* px) {
    return px->neighbors;
}

void px_set_color(PixelHandle px, const PixelGroup* pixel_group, Color color) {
    pixel_group->chains[px.chain].data[px.index].color = color;
}
Color px_get_color(PixelHandle px, const PixelGroup* pixel_group) {
    return pixel_group->chains[px.chain].data[px.index].color;
}

void px_set_position(PixelHandle px, const PixelGroup* pixel_group, float x, float y) {
    pixel_group->chains[px.chain].data[px.index].x = x;
    pixel_group->chains[px.chain].data[px.index].y = y;
}
// count is inclusive
void px_set_strip_positions(PixelHandle px, const PixelGroup* pixel_group, unsigned count, float x1, float y1, float x2, float y2) {
    float dx = (x2 - x1) / count;
    float dy = (y2 - y1) / count;
    for (unsigned i = 0; i <= count; i++) { // inclusive loop [0, count]
        pixel_group->chains[px.chain].data[px.index].x = x1 + dx * i;
        pixel_group->chains[px.chain].data[px.index].y = y1 + dy * i;
    }
}
void px_get_position(PixelHandle px, const PixelGroup* pixel_group, float* out_x, float* out_y) {
    *out_x = pixel_group->chains[px.chain].data[px.index].x;
    *out_y = pixel_group->chains[px.chain].data[px.index].y;
}