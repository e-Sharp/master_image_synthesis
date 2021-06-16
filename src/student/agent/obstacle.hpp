#pragma once

#include "student/cylindrical_coordinates.hpp"

#include "gKit/mat.h"
#include "gKit/mesh.h"
#include "gKit/window.h"

namespace stu {

struct Obstacle {
	CylindricalCoordinates coords = {};
	static Mesh mesh;
};

}
