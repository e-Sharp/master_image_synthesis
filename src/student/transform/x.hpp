#pragma once

#include "src/gKit/mat.h"

namespace stu {

Vector x(const Transform& t) {
    return Vector(t.m[0][0], t.m[1][0], t.m[2][0]);
}

void x(Transform& t, const Vector& v) {
    t.m[0][0] = v.x;
    t.m[1][0] = v.y;
    t.m[2][0] = v.z;
}

}
