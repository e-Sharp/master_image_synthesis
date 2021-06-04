#pragma once

#include "src/gKit/mat.h"

namespace stu {

Transform operator-(const Transform &a, const Transform &b) {
    auto r = Transform();
    for (auto i = 0; i < 4; ++i)
    for (auto j = 0; j < 4; ++j) {
        r.m[i][j] = a.m[i][j] - b.m[i][j];
    }
    return r;
}

}
