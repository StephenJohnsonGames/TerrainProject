#pragma once

#include <glm/vec2.hpp>

class NoiseInterface {

public:

	virtual const int* indexTable() const = 0;
	virtual const glm::vec2* vectorTable() const = 0;
	virtual int domainSize() const = 0;
	virtual float noise(float x, float y, float scale, glm::vec2* dn_dxy) const = 0;
};
