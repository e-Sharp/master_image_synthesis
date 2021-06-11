#pragma once

#include "src/gKit/mat.h"

namespace stu {

Vector y(const Transform& t) {
    return Vector(t.m[0][1], t.m[1][1], t.m[2][1]);
}

void y(Transform& t, const Vector& v) {
    t.m[0][1] = v.x;
    t.m[1][1] = v.y;
    t.m[2][1] = v.z;
}

}
