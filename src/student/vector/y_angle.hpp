#pragma once

#include "student/constant/deg_per_rad.hpp"

#include "gKit/vec.h"

#include <cmath>
#include <stdexcept>

namespace stu {

bool has_y_angle(Vector v) {
    auto e = 0.001f;
    return std::abs(v.x) > e || std::abs(v.y) > e;
}

float y_angle(Vector v) {
    if(!has_y_angle(v)) throw std::logic_error("Precondition violation.");
    return deg_per_rad * std::atan2(v.x, v.z);
}

}
