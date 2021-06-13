#pragma once

#include "src/gKit/mat.h"

namespace stu {

Vector fw(const Transform& t) {
    return Vector(
        t.m[0][0], 
        t.m[1][0], 
        t.m[2][0]);
}

void fw(Transform& t, Point p) {
    t.m[0][0] = p.x;
    t.m[1][0] = p.y;
    t.m[2][0] = p.z;
}

void fw(Transform& t, Vector v) {
    t.m[0][0] = v.x;
    t.m[1][0] = v.y;
    t.m[2][0] = v.z;
}

}
