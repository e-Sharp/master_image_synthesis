#pragma once

#include "src/student/transform/scalar_quotient.hpp"
#include "src/student/transform/scalar_sum.hpp"

#include "src/gKit/mat.h"

namespace stu {

Transform rotation(Vector from, Vector to) {
    // Credits to 'https://math.stackexchange.com/a/476311'.
    auto v = cross(from, to);
    auto sscp = Transform(
           0, -v.z,  v.y, 0,
         v.z,    0, -v.x, 0,
        -v.y,  v.x,    0, 0,
           0,    0,    0, 0);
    return Identity() + sscp + sscp * sscp / (1 + dot(from, to));
}

}
