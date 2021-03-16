

#pragma once

#include "NoiseInterface.hpp"


class PerlinNoise : public NoiseInterface {

private:

	int						N; // domain size
	int						Nmask; // domain mask (N-1)

	int*					iTable; // self-referential index table in range [0, N)
	glm::vec2*				vTable; // ordered vector table (can be randomized if need be - Perlin's spherical vector approach adopted here)


	//
	// Private API
	//

	void getGradient(const int x, const int y, glm::vec2* p) const; // return the gradient vector from lattice point (x % N, y % N)

	float smoothstep(const float t) const; // sigmoidal weighting of gradient coefficient x
	float smoothstep_d1(const float t) const; // first-derivative of smoothstep

public:

	PerlinNoise(const unsigned int domainSize = 256); // constructor
	~PerlinNoise(); // destructor

	// NoiseInterface
	const int* indexTable() const;
	const glm::vec2* vectorTable() const;
	int domainSize() const;

	// return the gradient noise value at domain coordinates (x*scale, y*scale)
	// if dn_dxy is not null, return the partial derivatives of the noise on x, y
	float noise(float x, float y, float scale, glm::vec2* dn_dxy) const;
};
