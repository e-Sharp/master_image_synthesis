#pragma once

#include "src/gKit/mat.h"

namespace stu {

Vector right(const Transform& t) {
    return Vector(
        t.m[0][2], 
        t.m[1][2], 
        t.m[2][2]);
}

void right(Transform& t, Point p) {
    t.m[0][2] = p.x;
    t.m[1][2] = p.y;
    t.m[2][2] = p.z;
}

void right(Transform& t, Vector v) {
    t.m[0][2] = v.x;
    t.m[1][2] = v.y;
    t.m[2][2] = v.z;
}

}
