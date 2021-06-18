#pragma once

#include "src/student/cylindrical_coordinates.hpp"
#include "src/student/constant/all.hpp"

#include "src/gKit/mat.h"
#include "src/gKit/mesh.h"
#include "src/gKit/window.h"

#include <cmath>
#include <functional>

namespace stu {

struct Obstacle {
	
	void draw(Transform m, Transform v, Transform p) {

	}

	void update(float time) {
		coords = movement(coords, time);
	}

	CylindricalCoordinates coords = {};
	static Mesh mesh;
	std::function<CylindricalCoordinates(CylindricalCoordinates, float)> movement
	= [](CylindricalCoordinates coords, float) { return coords; };
};

template<typename Output>
auto gear(Output o, float coord, int count) {
	auto offset = rand() % 360;
	auto velocity = (std::rand() % 6 + 1) * (std::rand() % 2 * 2 - 1) / 6.f;
	for(int i = 0; i < count; ++i) {
		auto obs = Obstacle();
		obs.coords.azimuth = 360.f * i / count + offset;
		obs.coords.coordinate = coord;
		obs.coords.radius = 2.f;
		obs.movement = [velocity](CylindricalCoordinates c, float t) {
            c.azimuth += velocity;
            return c;
        };
		*o++ = obs;
	}
}

template<typename Output>
auto rotor(Output o, float coord, int count) {
	auto offset = rand() % 360;
	auto velocity = (std::rand() % 6 + 1) * (std::rand() % 2 * 2 - 1) / 6.f;
	std::cout << velocity << std::endl;
	for(int i = 0; i < count; ++i) {
		auto obs = Obstacle();
		obs.coords.azimuth = 360.f * i / count + offset;
		obs.coords.coordinate = coord;
		obs.coords.radius = 2.f;
		obs.movement = [velocity, azimuth = obs.coords.azimuth](CylindricalCoordinates c, float t) {
            c.radius = 2.f + 1.5f * (std::cos(velocity * t / 500.f + azimuth * rad_per_deg) + 1.f);
            return c;
        };
		*o++ = obs;
	}
}

}
