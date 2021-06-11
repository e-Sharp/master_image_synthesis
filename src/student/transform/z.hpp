#pragma once

#include "src/gKit/mat.h"

namespace stu {

Vector z(const Transform& t) {
    return Vector(t.m[0][2], t.m[1][2], t.m[2][2]);
}

void z(Transform& t, const Vector& v) {
    t.m[0][2] = v.x;
    t.m[1][2] = v.y;
    t.m[2][2] = v.z;
}

}
