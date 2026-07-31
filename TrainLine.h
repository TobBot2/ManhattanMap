#ifndef MANHATTANMAP_TRAINLINE_H
#define MANHATTANMAP_TRAINLINE_H

#include <stdlib.h>

#include "Pixel.h"
#include "AvlTree.h"

typedef enum {
    LINE_1,
    LINE_2,
    LINE_3,

    LINE_A,
    LINE_C,
    LINE_E,

    LINE_B,
    LINE_D,
    LINE_F,
    LINE_M,

    LINE_4,
    LINE_5,
    LINE_6,

    LINE_Q,
    LINE_N,
    LINE_R,
    LINE_W,

    LINE_7,
    LINE_L,
    LINE_S,
} TrainLineName;

typedef struct TrainLine_t {
    TrainLineName name;
    PixelHandle* pixels;
    unsigned pixels_cnt;
} TrainLine;

// avl tree
typedef struct avl_tree PixelTrainLineMap;
typedef struct PixelTrainLineNode_t {
    struct avl_node avl;
    PixelHandle key;
    TrainLine* value;
} PixelTrainLineNode;

typedef struct Intersection_t {
    PixelHandle pixel;
    TrainLine* lines;
    unsigned lines_cnt;
} Intersection;

// avl tree
typedef struct avl_tree IntersectionSet;
typedef struct IntersectionNode_t {
    struct avl_node avl;
    Intersection key;
} IntersectionNode;


// COLOR

Color tl_name_to_color(TrainLineName line);

// TRAIN LINE

void tl_init(TrainLine* tl, TrainLineName line);
// add array of pixels (copied)
void tl_add_pixels(TrainLine* tl, const PixelHandle* pixels, unsigned count, PixelTrainLineMap* pxtl);
// add contiguous segment of pixels to train line in range [start, end]
void tl_add_segment(TrainLine* tl, PixelHandle start, PixelHandle end, PixelTrainLineMap* pxtl);
// get neighboring pixel along train line (from train line pixels array index)
PixelHandle tl_get_neighbor(const TrainLine* tl, unsigned* pixel_idx, int forward);
// get neighboring pixel in opposite direction of previous (neighbor in continuing direction)
PixelHandle tl_get_next_neighbor(const TrainLine* tl, unsigned* pixel_idx, unsigned* prev_idx);
// get neighboring pixel on any train line
PixelHandle tl_get_random_neighbor(PixelHandle pixel);
// get neighboring pixel on any train line in any direction except previous (don't back-track)
PixelHandle tl_get_random_next_neighbor(PixelHandle pixel, PixelHandle prev);

// PIXEL TRAIN LINE MAP

void pxtl_init(PixelTrainLineMap* pxtl);
// don't allow manual insertion by user... (insert automatically when registering train lines with pixels
static void pxtl_insert(PixelTrainLineMap* pxtl, PixelHandle px, TrainLine* tl); // TODO move static func declarations to src file
TrainLine* pxtl_search(PixelTrainLineMap* pxtl, PixelHandle px);
static int pxtl_compare(struct avl_node* lhs, struct avl_node* rhs, void* aux);

// INTERSECTION

Intersection intx_create_empty();
int intx_is_empty(const Intersection* intx); // boolean return

// INTERSECTION SET

void intx_set_init(IntersectionSet* intx_set);
void intx_set_insert(IntersectionSet* intx_set, Intersection intx);
Intersection intx_set_search(IntersectionSet* intx_set, PixelHandle px);
// no deletion function (not necessary for this project as tree survives full process lifetime)
// don't expose avl comparison operator
static int intx_set_compare(struct avl_node* lhs, struct avl_node* rhs, void* aux);

#endif //MANHATTANMAP_TRAINLINE_H