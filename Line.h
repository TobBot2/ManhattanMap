//
// Created by trevo on 5/25/2026.
//

#ifndef MANHATTANMAP_LINE_H
#define MANHATTANMAP_LINE_H

#include <cstdint>

#include "Light.h"

class Line {
public:
    static Line generate_range(int start, int end);
    static void append_range(Line* line, int start, int end);

    Line(Light* lights, unsigned lightsCount);

    int getNextLight(int current, bool forward);

private:
    int* lights;
    unsigned lightsCount;

    int* intersectionLights;
    unsigned intersectionLightsCount;
};


#endif //MANHATTANMAP_LINE_H