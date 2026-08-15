#include "TrainLine.h"

Color tl_name_to_color(TrainLineName line) {
    switch (line) {
        case LINE_1:
        case LINE_2:
        case LINE_3:
            return (Color){ .r = 0xEE, .g = 0x35, .b = 0x2E };

        case LINE_A:
        case LINE_C:
        case LINE_E:
            return (Color){ .r = 0x00, .g = 0x39, .b = 0xA6 };

        case LINE_4:
        case LINE_5:
        case LINE_6:
            return (Color){ .r = 0x00, .g = 0x93, .b = 0x3C };

        case LINE_B:
        case LINE_D:
        case LINE_F:
        case LINE_M:
            return (Color){ .r = 0xFF, .g = 0x63, .b = 0x19 };

        case LINE_Q:
        case LINE_N:
        case LINE_R:
        case LINE_W:
            return (Color){ .r = 0xFC, .g = 0xCC, .b = 0x0A };

        case LINE_7:
            return (Color){ .r = 0xB9, .g = 0x33, .b = 0xAD };

        case LINE_L:
            return (Color){ .r = 0xA7, .g = 0xA9, .b = 0xAC };

        case LINE_S:
            return (Color){ .r = 0x80, .g = 0x81, .b = 0x83 };

        default:
            return (Color){ .r = 0x00, .g = 0x00, .b = 0x00 };
    }
}

// ******************************************************************************
//                                 TRAIN LINE
// ******************************************************************************

void tl_init(TrainLine* tl, TrainLineName line) {
    tl->name = line;
    tl->pixels = NULL;
    tl->pixels_cnt = 0;
}

void tl_add_pixels(TrainLine* tl, const PixelHandle* pixels, unsigned count, PixelTrainLineMap* pxtl) {
    PixelIndex original_cnt = tl->pixels_cnt;

    tl->pixels_cnt += count;
    tl->pixels = realloc(tl->pixels, tl->pixels_cnt);
    for (int i = 0; i < count; i++) {
        tl->pixels[original_cnt + i] = pixels[i];
        pxtl_insert(pxtl, pixels[i], tl);
    }
}

void tl_add_segment(TrainLine* tl, PixelHandle start, PixelHandle end, PixelTrainLineMap* pxtl) {
    PixelIndex original_cnt = tl->pixels_cnt;

    // must be a part of the same chain and group
    if (start.group != end.group || start.chain != end.chain) {
        return;
    }

    // reserve space
    unsigned extra_space = end.index - start.index + 1; // + 1 for inclusive range [start, end]
    tl->pixels_cnt += extra_space;
    tl->pixels = realloc(tl->pixels, tl->pixels_cnt);

    // populate space
    for (PixelIndex i = 0; i < extra_space; i++) {
        tl->pixels[original_cnt + i].chain = start.chain;
        tl->pixels[original_cnt + i].index = start.index + i;
    }
}

PixelHandle tl_get_neighbor(const TrainLine* tl, unsigned* pixel_idx, int forward) {
    if (forward) *pixel_idx += 1;
    else *pixel_idx -= 1;

    if (*pixel_idx >= tl->pixels_cnt) {
        return px_create_empty();
    }

    return tl->pixels[*pixel_idx];
}

PixelHandle tl_get_next_neighbor(const TrainLine* tl, unsigned* pixel_idx, unsigned* prev_idx) {
    int forward = *pixel_idx > *prev_idx;
    *prev_idx = *pixel_idx;
    return tl_get_neighbor(tl, pixel_idx, forward);
}

PixelHandle tl_get_random_neighbor(PixelHandle pixel) {
    // TODO
}

PixelHandle tl_get_random_next_neighbor(PixelHandle pixel, PixelHandle prev) {
    // TODO
}

// ******************************************************************************
//                                INTERSECTION
// ******************************************************************************

void pxtl_init(PixelTrainLineMap* pxtl) {
    avl_init(pxtl, NULL);
}

void pxtl_insert(PixelTrainLineMap* pxtl, PixelHandle px, TrainLine* tl) {
    PixelTrainLineNode* node = malloc(sizeof(PixelTrainLineNode));
    node->key = px;
    node->value = tl;
    avl_insert(pxtl, &node->avl, pxtl_compare);
}

TrainLine* pxtl_search(PixelTrainLineMap* pxtl, PixelHandle px) {
    PixelTrainLineNode query;
    query.key = px;

    struct avl_node* found = avl_search(pxtl, &query.avl, pxtl_compare);
    if (!found) return NULL;

    return _get_entry(found, PixelTrainLineNode, avl)->value;
}

int pxtl_compare(struct avl_node* lhs, struct avl_node* rhs, void* aux) {
    PixelTrainLineNode* l_pxtl = _get_entry(lhs, PixelTrainLineNode, avl);
    PixelTrainLineNode* r_pxtl = _get_entry(rhs, PixelTrainLineNode, avl);

    return px_compare(l_pxtl->key, r_pxtl->key);
}

// ******************************************************************************
//                                INTERSECTION
// ******************************************************************************

Intersection intx_create_empty() {
    return (Intersection){
        .lines = NULL,
        .lines_cnt = 0,
        .pixel = px_create_empty()
    };
}

int intx_is_empty(const Intersection* intx) {
    return intx->lines == NULL;
}

void intx_assign_train_lines(Intersection* intx, int lines_cnt, ...) {
    va_list args;

    intx->lines_cnt = lines_cnt;
    intx->lines = malloc(lines_cnt * sizeof(TrainLine*));
    va_start(args, lines_cnt);
    for (int i = 0; i < lines_cnt; i++) {
        intx->lines[i] = va_arg(args, TrainLine*);
    }
    va_end(args);
}

// ******************************************************************************
//                              INTERSECTION SET
// ******************************************************************************

void intx_set_init(IntersectionSet* intx_set) {
    avl_init(intx_set, NULL);
}

void intx_set_insert(IntersectionSet* intx_set, Intersection intx) {
    IntersectionNode* node = malloc(sizeof(IntersectionNode));
    node->key = intx;
    avl_insert(intx_set, &node->avl, intx_set_compare);
}

Intersection intx_set_search(IntersectionSet* intx_set, PixelHandle px) {
    IntersectionNode query;
    query.key = intx_create_empty();
    query.key.pixel = px;

    struct avl_node* found = avl_search(intx_set, &query.avl, intx_set_compare);
    if (!found) return intx_create_empty();

    return _get_entry(found, IntersectionNode, avl)->key;
}

int intx_set_compare(struct avl_node* lhs, struct avl_node* rhs, void* aux) {
    IntersectionNode* l_intx = _get_entry(lhs, IntersectionNode, avl);
    IntersectionNode* r_intx = _get_entry(rhs, IntersectionNode, avl);

    return px_compare(l_intx->key.pixel, r_intx->key.pixel);
}