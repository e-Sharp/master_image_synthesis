#pragma once

#include "student/cylindrical_coordinates.hpp"

#include "gKit/mat.h"
#include "gKit/mesh.h"
#include "gKit/window.h"

namespace stu {

struct Player {
	Player() {
		
	}

	// Controls.

	void accelerate() {
		
	}

	void brake() {
	
	}

	void turn_left() {
		coords.azimuth -= 0.01f;
	}

	void turn_right() {
		coords.azimuth += 0.01f;
	}

	void update() {
		coords.coordinate += speed;
	}

	float speed = 0.01f;

	CylindricalCoordinates coords = {};
	Mesh mesh = read_mesh(smart_path("data/cube.obj"));
	Transform transform = Identity();
};

}
