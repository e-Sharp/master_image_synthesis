#ifndef MIF27_BOX_HPP
#define MIF27_BOX_HPP

#include "vec.h"
#include "mat.h"

#include <cmath>

class Box {
  public :
    Box() ;
    Box(const Point& pmin, const Point& pmax) ;

    bool collides(const Box& rhs) const;

    Point pmin, pmax ;
    Transform T ;
} ;

inline
Point corner(const Box& b, const Vector& dir) {
    return Point(
        dir.x < 0 ? b.pmax.x : b.pmin.x,
        dir.y < 0 ? b.pmax.y : b.pmin.y,
        dir.z < 0 ? b.pmax.z : b.pmin.z);
}

#endif
