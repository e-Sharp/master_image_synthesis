#pragma once

#include "gKit/mat.h"

namespace stu {

Vector w(const Transform& t) {
    return Vector(t.m[0][3], t.m[1][3], t.m[2][3]);
}

void w(Transform& t, const Vector& v) {
    t.m[0][3] = v.x;
    t.m[1][3] = v.y;
    t.m[2][3] = v.z;
}

}
