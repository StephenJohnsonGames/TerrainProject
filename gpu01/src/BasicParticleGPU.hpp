#pragma once

// Model data structure for basic particle but aligned for GPU buffer storage.
struct alignas(16) BasicParticleGPU {

	alignas(16) glm::vec3		pos; // ...in world coordinate space
	alignas(16) glm::vec3		velocity;
	alignas(4) GLfloat			age;
	alignas(4) GLint			isaGenerator; // 4 byte int to align with GLSL bool

	BasicParticleGPU() {

		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		velocity = glm::vec3(0.0f, 0.0f, 0.0f);
		age = 0.0f;
		isaGenerator = GL_FALSE;
	}

	BasicParticleGPU(glm::vec3 pos, glm::vec3 velocity, GLfloat age, GLint isaGenerator) {

		this->pos = pos;
		this->velocity = velocity;
		this->age = age;
		this->isaGenerator = isaGenerator;
	}

	void print() {

		std::cout << "pos = (" << pos.x << ", " << pos.y << ", " << pos.z << "); age = " << age << "; generator = " << isaGenerator << std::endl;
	}
};