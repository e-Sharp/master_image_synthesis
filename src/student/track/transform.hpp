#pragma once

#include "track.hpp"

#include "src/student/cylindrical_coordinates.hpp"
#include "src/student/mod.hpp"

namespace stu {

inline
Transform transform(const Track& t, float coord) {
	auto s = float(t.nodes.size());
    coord = mod(coord, float(s - 1));
    float i;
    auto f = std::modf(coord, &i);
    return (1.f - f) * t.nodes[i] + f * t.nodes[i + 1];
}

inline
Transform transform(const Track& t, CylindricalCoordinates coords) {
    return transform(t, coords.coordinate) 
    * RotationX(coords.azimuth) 
    * Translation(0, coords.radius, 0);
}

}
