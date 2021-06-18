#pragma once

#include "src/student/cylindrical_coordinates.hpp"

#include "src/gKit/mat.h"
#include "src/gKit/mesh.h"
#include "src/gKit/window.h"

namespace stu {

struct Coin {
	CylindricalCoordinates coords = {};
	static Mesh mesh;
    bool touched = false;
};

}
