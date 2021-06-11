#pragma once

#include "src/student/constant/deg_per_rad.hpp"

#include "src/gKit/vec.h"

#include <cmath>
#include <stdexcept>

namespace stu {

bool has_z_angle(Vector v) {
    auto e = 0.001f;
    return std::abs(v.x) > e || std::abs(v.y) > e;
}

float z_angle(Vector v) {
    if(!has_z_angle(v)) throw std::logic_error("Precondition violation.");
    return deg_per_rad * std::atan2(v.y, v.x);
}

}
