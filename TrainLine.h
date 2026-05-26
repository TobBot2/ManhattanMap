//
// Created by trevo on 5/26/2026.
//

#ifndef MANHATTANMAP_TRAINLINE_H
#define MANHATTANMAP_TRAINLINE_H

#include "util.h"

enum TrainLineName {
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
};

class TrainLine {
public:
    static std::vector<LightIndex> generateTrainLineLights(std::vector<std::pair<LightIndex, LightIndex>> pairs) {
        std::vector<LightIndex> lights;
        for (auto& [first, last] : pairs) {
            for (LightIndex i = first; i <= last; i++)
                lights.push_back(i);
        }
        return lights;
    }

    void setLights(std::vector<LightIndex> _lights) {
        lights = _lights;
    }

    std::vector<LightIndex>& getLights() {
        return lights;
    }
private:
    std::vector<LightIndex> lights;
};

#endif //MANHATTANMAP_TRAINLINE_H