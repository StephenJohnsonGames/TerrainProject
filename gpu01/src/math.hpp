#pragma once

namespace cst {

	inline bool fequal(float a, float b) {

		return (std::abs(a - b) < std::numeric_limits<float>::epsilon());
	}

}