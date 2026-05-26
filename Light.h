//
// Created by trevo on 5/25/2026.
//

#ifndef MANHATTANMAP_LIGHT_H
#define MANHATTANMAP_LIGHT_H

#include "util.h"

class Light {
public:
    Light()
        : pos(-1.f, -1.f)
    { }

    void setPosition(vmath_hpp::fvec2 _pos) {
        pos = _pos;
    }
    vmath_hpp::fvec2 getPosition() { return pos; }

    void setIsIntersection(bool value = true) {
        isIntersection = value;
    }
    bool getIsIntersection() { return isIntersection; }

    void setColor(Color _color) {
        color = _color;
    }
    Color getColor() { return color; }
private:
    vmath_hpp::fvec2 pos;

    bool isIntersection = false;
    Color color;
};


#endif //MANHATTANMAP_LIGHT_H