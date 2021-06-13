#ifndef MIF27_BOX_HPP
#define MIF27_BOX_HPP

#include "vec.h"
#include "mat.h"

#include <cmath>

struct Box {
	Box() = default;

	Box(Point p) {
		pmin = Point(p.x / 2.f, p.y / 2.f, p.z / 2.f);
		pmax = Point(-p.x / 2.f, -p.y / 2.f, -p.z / 2.f);
	}

    Point pmin = Point(-0.5, -0.5, -0.5);
	Point pmax = Point(0.5, 0.5, 0.5);
    Transform T = Identity();
};

Point corner(const Box& b, const Vector& dir) {
    return Point(
        dir.x < 0 ? b.pmax.x : b.pmin.x,
        dir.y < 0 ? b.pmax.y : b.pmin.y,
        dir.z < 0 ? b.pmax.z : b.pmin.z);
}

bool collides(const Box& lhs, const Box& rhs) {
    auto l_to_r = rhs.T.inverse() * lhs.T;
	auto r_to_l = lhs.T.inverse() * rhs.T;

	auto l_corner = [&](auto dir) { return r_to_l(corner(rhs, l_to_r(dir))); };
	auto r_corner = [&](auto dir) { return l_to_r(corner(lhs, r_to_l(dir))); };

	return (lhs.pmax.x >= l_corner(Vector( 1,  0,  0)).x)
		&& (lhs.pmin.x <= l_corner(Vector(-1,  0,  0)).x)
		&& (lhs.pmax.y >= l_corner(Vector( 0,  1,  0)).y)
		&& (lhs.pmin.y <= l_corner(Vector( 0, -1,  0)).y)
		&& (lhs.pmax.z >= l_corner(Vector( 0,  0,  1)).z)
		&& (lhs.pmin.z <= l_corner(Vector( 0,  0, -1)).z)
	
		&& (rhs.pmax.x >= r_corner(Vector( 1,  0,  0)).x)
		&& (rhs.pmin.x <= r_corner(Vector(-1,  0,  0)).x)
		&& (rhs.pmax.y >= r_corner(Vector( 0,  1,  0)).y)
		&& (rhs.pmin.y <= r_corner(Vector( 0, -1,  0)).y)
		&& (rhs.pmax.z >= r_corner(Vector( 0,  0,  1)).z)
		&& (rhs.pmin.z <= r_corner(Vector( 0,  0, -1)).z);
}

#endif
