#pragma once

#include <cmath>

namespace maths {

template<typename Numeric> constexpr
Numeric pmod(const Numeric& numerator, const Numeric& denominator) {
	return numerator - std::floor(numerator / denominator) * denominator;
}

}
