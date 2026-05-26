//
// Created by trevo on 5/25/2026.
//

#ifndef MANHATTANMAP_LIGHT_H
#define MANHATTANMAP_LIGHT_H


class Light {
public:
    Light(int index, bool isIntersection)
        : index(index)
        , isIntersection(isIntersection)
    { }

    void setIntersection(bool value = true) {
        isIntersection = value;
    }
private:
    int index;
    bool isIntersection;
};


#endif //MANHATTANMAP_LIGHT_H