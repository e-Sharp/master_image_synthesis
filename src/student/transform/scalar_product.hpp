#pragma once

#include "src/gKit/mat.h"

namespace stu {

Transform operator*(const Transform &a, float b) {
    auto r = Transform();
    for (auto i = 0; i < 4; ++i)
    for (auto j = 0; j < 4; ++j) {
        r.m[i][j] = a.m[i][j] * b;
    }
    return r;
}

Transform operator*(float a, const Transform &b) {
    auto r = Transform();
    for (auto i = 0; i < 4; ++i)
    for (auto j = 0; j < 4; ++j) {
        r.m[i][j] = a * b.m[i][j];
    }
    return r;
}


}
