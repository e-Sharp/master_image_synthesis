#ifndef MIF27_BOX_HPP
#define MIF27_BOX_HPP

#include "vec.h"
#include "mat.h"

#include <cmath>

class Box {
  public :
    Box() = default;

    Box(const Point& i_pmin, const Point& i_pmax) {
		pmin = i_pmin ;
		pmax = i_pmax ;
	}

    bool collides(const Box& rhs) const;

    Point pmin = Origin(), pmax = Origin();
    Transform T = Identity();
} ;

inline
Point corner(const Box& b, const Vector& dir) {
    return Point(
        dir.x < 0 ? b.pmax.x : b.pmin.x,
        dir.y < 0 ? b.pmax.y : b.pmin.y,
        dir.z < 0 ? b.pmax.z : b.pmin.z);
}

#endif
