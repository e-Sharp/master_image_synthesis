#include "box.hpp"

#include <iostream>

Box::Box() {
  pmin = Origin() ;
  pmax = Origin() ;
  T = Identity() ;
}

Box::Box(const Point& i_pmin, const Point& i_pmax) {
  pmin = i_pmin ;
  pmax = i_pmax ;
  T = Identity() ;
}

bool Box::collides(const Box& rhs) const {
	auto l_to_r = rhs.T.inverse() * T;
	auto r_to_l = T.inverse() * rhs.T;

	auto l_corner = [&](auto dir) { return r_to_l(corner(rhs, l_to_r(dir))); };
	auto r_corner = [&](auto dir) { return l_to_r(corner(rhs, r_to_l(dir))); };

	return (pmax.x >= l_corner(Vector(1, 0, 0)).x)
		&& (pmin.x <= l_corner(Vector(-1, 0, 0)).x)
		&& (pmax.y >= l_corner(Vector(0, 1, 0)).y)
		&& (pmin.y <= l_corner(Vector(0, -1, 0)).y)
		&& (pmax.z >= l_corner(Vector(0, 0, 1)).z)
		&& (pmin.z <= l_corner(Vector(0, 0, -1)).z)
	
		&& (rhs.pmax.x >= r_corner(Vector(1, 0, 0)).x)
		&& (rhs.pmin.x <= r_corner(Vector(-1, 0, 0)).x)
		&& (rhs.pmax.y >= r_corner(Vector(0, 1, 0)).y)
		&& (rhs.pmin.y <= r_corner(Vector(0, -1, 0)).y)
		&& (rhs.pmax.z >= r_corner(Vector(0, 0, 1)).z)
		&& (rhs.pmin.z <= r_corner(Vector(0, 0, -1)).z);
}
