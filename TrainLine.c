#include "TrainLine.h"

#include <stdio.h>

#include "Map.h"

static int pxtl_map_compare(struct avl_node* lhs, struct avl_node* rhs, void* aux);

// ******************************************************************************
//                                 TRAIN LINE
// ******************************************************************************

void tl_init(TrainLine* tl, Color color) {
    tl->color = color;
    tl->pixels = NULL;
    tl->pixels_cnt = 0;
}

void tl_add_segment(TrainLine* tl, PixelHandle start, PixelHandle end, PixelTrainLinesMap* pxtl_map) {
    PixelIndex original_cnt = tl->pixels_cnt;

    if (start.group != end.group) return; // must be a part of the same group
    if (px_compare(end, start) < 0) return; // end must be after start


    // reserve space
    unsigned extra_space = end.index - start.index + 1; // + 1 for inclusive range [start, end]
    tl->pixels_cnt += extra_space;
    tl->pixels = realloc(tl->pixels, tl->pixels_cnt * sizeof(PixelHandle));

    // populate space
    for (PixelIndex i = 0; i < extra_space; i++) {
        tl->pixels[original_cnt + i].chain = start.chain;
        tl->pixels[original_cnt + i].index = start.index + i;
        pxtl_map_associate_line(pxtl_map, tl->pixels[original_cnt + i], tl);
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

PixelHandle tl_get_random_neighbor(PixelHandle pixel, const PixelTrainLinesMap* pxtl_map) {
    PixelTrainLinesNode* pxtl_node = pxtl_map_search(pxtl_map, pixel);
    TrainLine* tl = NULL;

    for (unsigned try = 0; try < 10; try++) {
        // set train line to one in pxtl_node at random
        int rand_line = rand() % pxtl_node->lines_cnt;
        tl = pxtl_node->lines[rand_line];

        // find pixel in train line it is a part of
        for (unsigned px = 0; px < tl->pixels_cnt; px++) {
            if (px_compare(tl->pixels[px], pixel) == 0) {
                // go in random direction along chosen train line
                int is_forward = rand() % 2;
                int step_dir = is_forward ? 1 : -1;
                PixelHandle candidate = tl->pixels[px + step_dir];

                // if it's valid, return it
                if (px_is_valid(candidate)) {
                    return candidate;
                }

                // invalid -> keep searching
            }
        }
    }
    return px_create_empty();
}

PixelHandle tl_get_random_next_neighbor(PixelHandle pixel, PixelHandle prev, const PixelTrainLinesMap* pxtl_map) {
    PixelHandle neighbor;
    for (unsigned try = 0; try < 20; try++) {
        // ensure next != prev
        if (px_compare(neighbor = tl_get_random_neighbor(pixel, pxtl_map), prev) != 0) {
            return neighbor;
        }
    }
    // no valid next (or couldn't find one, at least)...
    return px_create_empty();
}

// ******************************************************************************
//                              INTERSECTION SET
// ******************************************************************************

void pxtl_map_init(PixelTrainLinesMap* pxtl_map) {
    avl_init(pxtl_map, NULL);
}

void pxtl_map_associate_line(PixelTrainLinesMap* pxtl_map, PixelHandle px, TrainLine* tl) {
    // create new (empty) node to insert into tree
    PixelTrainLinesNode* node = malloc(sizeof(PixelTrainLinesNode));
    node->key = px;
    node->lines = NULL;
    node->lines_cnt = 0;

    // insert into tree and check if success (or if the node already exists)
    struct avl_node* existing_node = avl_insert(pxtl_map, &node->avl, pxtl_map_compare);
    if (existing_node != &node->avl) {
        // node already exists in tree, free new node and point node to existing node
        free(node);
        node = _get_entry(existing_node, PixelTrainLinesNode, avl);
    }

    // add train line to node
    node->lines_cnt ++;
    node->lines = realloc(node->lines, sizeof(*node->lines) * node->lines_cnt);
    node->lines[node->lines_cnt - 1] = tl;
}

PixelTrainLinesNode* pxtl_map_search(const PixelTrainLinesMap* pxtl_map, PixelHandle px) {
    PixelTrainLinesNode query;
    query.key = px;

    struct avl_node* found = avl_search(pxtl_map, &query.avl, pxtl_map_compare);
    if (!found) return NULL;

    return _get_entry(found, PixelTrainLinesNode, avl);
}

int pxtl_map_compare(struct avl_node* lhs, struct avl_node* rhs, void* aux) {
    PixelTrainLinesNode* l_intx = _get_entry(lhs, PixelTrainLinesNode, avl);
    PixelTrainLinesNode* r_intx = _get_entry(rhs, PixelTrainLinesNode, avl);

    return px_compare(l_intx->key, r_intx->key);
}