#pragma once

#include "src/gKit/mat.h"

namespace stu {

Point pos(const Transform& t) {
    return Point(t.m[0][3], t.m[1][3], t.m[2][3]);
}

void pos(Transform& t, const Point& p) {
    t.m[0][3] = p.x;
    t.m[1][3] = p.y;
    t.m[2][3] = p.z;
}

void pos(Transform& t, const Vector& v) {
    t.m[0][3] = v.x;
    t.m[1][3] = v.y;
    t.m[2][3] = v.z;
}

}
