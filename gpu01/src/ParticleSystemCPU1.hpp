#pragma once

// Create an entirely CPU-bound particle system.  This uses double buffering an fully-allocated vectors to eliminate the need to memory operations at run-time
// Rendering is fixed-function - no shaders are used here.  Only the basic operations of update, create and draw is illustrated.


#include "ArcballCamera.hpp"
#include "RandomEngine.hpp"


struct BasicParticle {

	glm::vec3		pos; // ...in world coordinate space
	glm::vec3		velocity;
	GLfloat			age;
	GLboolean		isaGenerator;
	
	GLfloat max_age = 8.0f;
	GLfloat generateAge = 0.02f; // frequency of particle generation (time interval between generation events)

	BasicParticle() {

		pos = glm::vec3(0.0f, 0.0f, 0.0f);
		velocity = glm::vec3(0.0f, 0.0f, 0.0f);
		age = 0.0f;
		isaGenerator = false;
	}

	BasicParticle(glm::vec3 pos, glm::vec3 velocity, GLfloat age, GLboolean isaGenerator) {

		this->pos = pos;
		this->velocity = velocity;
		this->age = age;
		this->isaGenerator = isaGenerator;
	}

	// Update particle - return actual number of transferred particles to target
	GLuint update(std::vector<BasicParticle>& target, GLuint targetIndex, GLuint maxParticles, RandomEngine* randomEngine, float deltaTime) {

		age += deltaTime;

		if (isaGenerator) {

			if (age >= generateAge) {

				// Output generator with age reset
				target[targetIndex].pos = pos;
				target[targetIndex].velocity = velocity;
				target[targetIndex].age = 0.0f;
				target[targetIndex].isaGenerator = isaGenerator;

				// Output new particle
				target[targetIndex + 1].pos = pos;
				target[targetIndex + 1].velocity = glm::vec3(randomEngine->getRandomValue(), randomEngine->getRandomValue(), randomEngine->getRandomValue());
				target[targetIndex + 1].age = 0.0f;
				target[targetIndex + 1].isaGenerator = false;
				
				return 2;
			}
			else {

				// Output latest generator state

				target[targetIndex].pos = pos;
				target[targetIndex].velocity = velocity;
				target[targetIndex].age = age;
				target[targetIndex].isaGenerator = isaGenerator;

				return 1;
			}
		}
		else {

			if (age < max_age) {

				target[targetIndex].pos = pos + velocity * deltaTime;
				target[targetIndex].velocity = velocity;
				target[targetIndex].age = age;
				target[targetIndex].isaGenerator = isaGenerator;

				return 1;
			}
			else {

				return 0;
			}
		}
	}

	void render() {

		if (isaGenerator) {

			glPointSize(10.0f);
			glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		}
		else {

			glPointSize(age * 30.0f);
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f - age / max_age );
		}
			

		glBegin(GL_POINTS);

		glVertex3fv((const GLfloat*)&(pos));

		glEnd();
	}
};


// Model CPU-bound particle system - Update and rendering handled from this system - GPU only used for rendering
class ParticleSystemCPU1 {

	enum class ParticleBufferTarget : std::uint8_t { BUFFER1, BUFFER2 };
	
	GLuint							maxParticles;

	std::vector<BasicParticle>		buffer1;
	std::vector<BasicParticle>		buffer2;

	ParticleBufferTarget			currentBuffer;
	GLuint							numLiveElements;
	
	RandomEngine*					engine = nullptr;

public:

	ParticleSystemCPU1(const GLuint maxParticles) {

		this->maxParticles = maxParticles;

		buffer1 = std::vector<BasicParticle>(maxParticles);
		buffer2 = std::vector<BasicParticle>(maxParticles);

		currentBuffer = ParticleBufferTarget::BUFFER1;

		// Seed current buffer with generator(s)
		buffer1[0] = BasicParticle(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, true);
		numLiveElements = 1;

		// Initialise random number generation engine
		engine = new RandomEngine(-1.0f, 1.0f);
	}

	void update(float deltaTime) {

		GLuint targetCount = 0;

		for (GLuint i = 0; i < numLiveElements; ++i) {

			if (currentBuffer == ParticleBufferTarget::BUFFER1) {

				targetCount += buffer1[i].update(buffer2, targetCount, maxParticles, engine, deltaTime);
			}
			else { // currentBuffer == ParticleBufferTarget::BUFFER2

				targetCount += buffer2[i].update(buffer1, targetCount, maxParticles, engine, deltaTime);
			}
		}

		if (targetCount != numLiveElements)
			std::cout << "particle count = " << targetCount << std::endl;

		numLiveElements = targetCount;

		// Swap buffers
		if (currentBuffer == ParticleBufferTarget::BUFFER1) {

			currentBuffer = ParticleBufferTarget::BUFFER2;
		}
		else { // currentBuffer == ParticleBufferTarget::BUFFER2

			currentBuffer = ParticleBufferTarget::BUFFER1;
		}
	}

	void renderBuffer(std::vector<BasicParticle>& buffer, cst::ArcballCamera* mainCamera) {

		glUseProgram(0);

		glm::mat4 viewMatrix = mainCamera->viewTransform();
		glm::mat4 projMatrix = mainCamera->projectionTransform();

		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf((const GLfloat*)&(projMatrix));
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf((const GLfloat*)&(viewMatrix));

		glEnable(GL_POINT_SMOOTH);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		for (GLuint i = 0; i < numLiveElements; ++i) {

			buffer[i].render();
		}

		glDisable(GL_POINT_SMOOTH);
		glDisable(GL_BLEND);
	}

	void render(cst::ArcballCamera *mainCamera) {

		if (currentBuffer == ParticleBufferTarget::BUFFER1) {

			renderBuffer(buffer1, mainCamera);
		}
		else { // currentBuffer == ParticleBufferTarget::BUFFER2

			renderBuffer(buffer2, mainCamera);
		}
	}
};
