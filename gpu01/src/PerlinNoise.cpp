
#include <pch.h>
#include "PerlinNoise.hpp"
#include "ctime"

using namespace std;


//
// Private API implementation
//


void PerlinNoise::getGradient(const int x, const int y, glm::vec2* p) const {

	*p = vTable[iTable[(iTable[x & Nmask] + iTable[y & Nmask]) & Nmask]];
}


// sigmoidal weighting of gradient coefficient x where 0 <= x <= 1
float PerlinNoise::smoothstep(const float t) const {

	// Interpolation with quintic 6t^5 - 15t^4 + 10t^3
	return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

float PerlinNoise::smoothstep_d1(const float t) const {
	
	return 30.0f * t * t * (t * (t - 2.0f) + 1.0f);
}


//
// PerlinNoise public method implementation
//

PerlinNoise::PerlinNoise(const unsigned int domainSize) {

	iTable = (int *)malloc(domainSize * sizeof(int));
	vTable = (glm::vec2*)malloc(domainSize * sizeof(glm::vec2));

	if (!iTable || !vTable) {

		// noise generator could not be created so set attributes to configure a null generator (always returns 0.0f for a noise request)

		N = 0;
		Nmask = 0;

		if (iTable) {

			free(iTable);
			iTable = NULL;
		}

		if (vTable) {

			free(vTable);
			vTable = NULL;
		}

	}
	else {

		N = domainSize;
		Nmask = N - 1;

		// seed random number generator with current system time
		srand((unsigned)time(NULL));
		rand();

		float theta = 0.0f;
		float pi2 = glm::pi<float>() * 2.0f;
		float angleStep = pi2 / (float)N;

		// initialise vector table
		for (int i = 0; i < N; i++) {

			iTable[i] = rand() % Nmask;

			vTable[i].x = cos(theta);
			vTable[i].y = sin(theta); // unit length gradient vector

			theta += angleStep;
		}
	}
}


PerlinNoise::~PerlinNoise() {

	if (iTable)
		free(iTable);

	if (vTable)
		free(vTable);
}



// GUNoise interface

const int* PerlinNoise::indexTable() const {

	return iTable;
}


const glm::vec2* PerlinNoise::vectorTable() const {

	return vTable;
}


int PerlinNoise::domainSize() const {

	return N;
}


float PerlinNoise::noise(float x, float y, float scale, glm::vec2* dn_dxy) const {

	x *= scale;
	y *= scale;
	
	float x0_ = floor(x);
	float y0_ = floor(y);

	int x0 = static_cast<int>(x0_);
	int y0 = static_cast<int>(y0_);

	float u = x - x0_;
	float v = y - y0_;

	float du_dx = smoothstep_d1(u);
	float dv_dy = smoothstep_d1(v);
	//float du_dx = 30 * u * u * (u * (u - 2) + 1);
	//float dv_dy = 30 * v * v * (v * (v - 2) + 1);

	/*
	x0,y0    x1,y0
	g00      g10
	 +-------+
	 |       |
	 +-------+
	 g01     g11
	 x0,y1   x1,y1
	*/

	glm::vec2 g00, g10, g01, g11;

	getGradient(x0, y0, &g00); //ga
	getGradient(x0 + 1, y0, &g10); //gb
	getGradient(x0, y0 + 1, &g01); //gc
	getGradient(x0 + 1, y0 + 1, &g11); //gd

	// dot product of vectors based on uv with gradients
	// given |g| = 1.0,
	// uv.g = |uv||g|cos(theta) = |uv|cos(theta) 
	float a = u * g00.x + v * g00.y;
	float b = (u - 1.0f) * g10.x + v * g10.y;
	float c = u * g01.x + (v - 1.0f) * g01.y;
	float d = (u - 1.0f) * g11.x + (v - 1.0f) * g11.y;

	u = smoothstep(u);
	v = smoothstep(v);
	
	// refactor bi-linear interpolation
	//float i0 = a * (1.0f - u) + b * u;
	//float i1 = c * (1.0f - u) + d * u;
	//float i2 = i0 * (1.0f - v) + i1 * v;
	float k0 = 1.0f - u - v + u * v;
	float k1 = u - u * v;
	float k2 = v - u * v;
	float k3 = u * v;
	float noise = a * k0 + b * k1 + c * k2 + d * k3;

	if (dn_dxy) {

		dn_dxy->x = g00.x * k0 + g10.x * k1 + g01.x * k2 + g11.x * k3 + du_dx * (b - a + (a - b - c + d) * v);
		dn_dxy->y = g00.y * k0 + g10.y * k1 + g01.y * k2 + g11.y * k3 + dv_dy * (c - a + (a - b - c + d) * u);
	}

	return noise;
}
