#pragma once

#include "src/gKit/mat.h"

namespace stu {

Point w(const Transform& t) {
    return Point(t.m[0][3], t.m[1][3], t.m[2][3]);
}

void w(Transform& t, const Vector& v) {
    t.m[0][3] = v.x;
    t.m[1][3] = v.y;
    t.m[2][3] = v.z;
}

}
