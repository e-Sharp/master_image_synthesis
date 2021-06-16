#pragma once

#include <cmath>

namespace stu {

template<typename Type> constexpr
Type mod(const Type& numerator, const Type& denominator) {
	return numerator - std::floor(numerator / denominator) * denominator;
}

}
