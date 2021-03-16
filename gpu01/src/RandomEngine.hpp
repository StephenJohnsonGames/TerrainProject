#pragma once

class RandomEngine {

	std::mt19937 engine;
	std::uniform_real_distribution<float> distribution;

public:

	RandomEngine(float min, float max) {

		std::random_device rd;

		engine = std::mt19937(rd());
		distribution = std::uniform_real_distribution<float>(min, max);
	}

	float getRandomValue() {

		return distribution(engine);
	}
};
