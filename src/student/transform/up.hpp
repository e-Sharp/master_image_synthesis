#pragma once

#include "src/gKit/mat.h"

namespace stu {

Vector up(const Transform& t) {
    return Vector(t.m[0][1], t.m[1][1], t.m[2][1]);
}

void up(Transform& t, Point p) {
    t.m[0][1] = p.x;
    t.m[1][1] = p.y;
    t.m[2][1] = p.z;
}

void up(Transform& t, Vector v) {
    t.m[0][1] = v.x;
    t.m[1][1] = v.y;
    t.m[2][1] = v.z;
}

}
