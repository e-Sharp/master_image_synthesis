#pragma once

#include "src/gKit/vec.h"

namespace stu {

Vector perpendicular(Vector v) {
    return Vector(v.y - v.z, v.z - v.x, v.x - v.y);
}

}
