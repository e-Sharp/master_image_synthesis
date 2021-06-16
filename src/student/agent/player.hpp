#pragma once

#include "src/student/box/all.hpp"
#include "src/student/cylindrical_coordinates.hpp"

#include "src/gKit/mat.h"
#include "src/gKit/mesh.h"
#include "src/gKit/window.h"

#include <cmath>

namespace stu {

struct Player {
	Player() {
		
	}

	// Controls.

	void accelerate() {
		accelerating = true;
	}

	void brake() {
		braking = true;
	}

	void turn_left() {
		turning_left = true;
	}

	void turn_right() {
		turning_right = true;
	}

	void update() {
		forward_acceleration = 0.01f * (accelerating - braking);
		if(turning_left) {
			angular_velocity -= angular_acceleration;
			angular_velocity = std::max(angular_velocity, -angular_speed_limit);
		}
		if(turning_right) {
			angular_velocity += angular_acceleration;
			angular_velocity = std::min(angular_velocity, angular_speed_limit);
		}
		if(!turning_left && !turning_right) {
			if(angular_velocity > 0) {
				angular_velocity -= angular_acceleration;
				angular_velocity = std::max(angular_velocity, 0.f);
			} else {
				angular_velocity += angular_acceleration;
				angular_velocity = std::min(angular_velocity, 0.f);
			}
		}

		forward_speed += forward_acceleration;
		forward_speed = std::max(forward_speed, forward_min_speed);
		forward_speed = std::min(forward_speed, forward_max_speed);

		coords.azimuth += angular_velocity;
		coords.coordinate += forward_speed;

		transform = Identity();
		transform = transform * RotationX(angular_velocity / angular_speed_limit * 15.f);
		transform = transform * RotationZ(-forward_acceleration * 720.f);

		accelerating = false;
		braking = false;
		turning_left = false;
		turning_right = false;
	}

	// State.
	bool accelerating = false;
	bool braking = false;
	bool turning_left = false;
	bool turning_right = false;

	// Angle.
	float angular_acceleration = 0.2f;
	float angular_speed_limit = 2.f;
	float angular_velocity = 0.f;

	// Speed.
	float forward_acceleration = 0.f;
	float forward_max_speed = 0.2f;
	float forward_min_speed = 0.1f;
	float forward_speed = 0.1f;

	Box collider = {};
	CylindricalCoordinates coords = {};
	Mesh mesh = read_mesh(smart_path("data/ship.obj"));
	Transform transform = Identity();
};

}
