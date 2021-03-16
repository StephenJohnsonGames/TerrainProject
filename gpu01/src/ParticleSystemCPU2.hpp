#pragma once

// Create a particle system that resides on the GPU and uses shaders to draw using eye-coord billboarding.  However, updating is still CPU bound.

#include "ArcballCamera.hpp"
#include "RandomEngine.hpp"
#include "GLShaderFactory.hpp"
#include "BasicParticleGPU.hpp"

class ParticleSystemCPU2 {

	GLuint							maxParticles;

	GLuint							buffer[2];

	GLuint							vao;

	GLuint							currentBufferIndex;
	GLuint							numLiveElements;

	RandomEngine*					engine = nullptr;

	GLfloat							max_age = 8.0f;
	GLfloat							generateAge = 0.02f; // frequency of particle generation (time interval between generation events)

	const GLuint					particleBufferBindPoint = 0;

	GLProgramObject					renderShader;

	// Private API

	// Update particle - return actual number of transferred particles to target
	GLuint updateParticle(BasicParticleGPU* sourceBuffer, GLuint i, BasicParticleGPU* targetBuffer, GLuint targetIndex, GLuint maxParticles, RandomEngine* randomEngine, GLfloat deltaTime) {

		GLfloat age = sourceBuffer[i].age + deltaTime;

		if (sourceBuffer[i].isaGenerator == GL_TRUE) {

			if (age >= generateAge) {

				// Output generator with age reset
				targetBuffer[targetIndex].pos = sourceBuffer[i].pos;
				targetBuffer[targetIndex].velocity = sourceBuffer[i].velocity;
				targetBuffer[targetIndex].age = 0.0f;
				targetBuffer[targetIndex].isaGenerator = GL_TRUE;
				
				// Output new particle
				targetBuffer[targetIndex + 1].pos = sourceBuffer[i].pos;
				targetBuffer[targetIndex + 1].velocity = glm::vec3(randomEngine->getRandomValue(), randomEngine->getRandomValue(), randomEngine->getRandomValue());
				targetBuffer[targetIndex + 1].age = 0.0f;
				targetBuffer[targetIndex + 1].isaGenerator = GL_FALSE;

				return 2;
			}
			else {

				// Output latest generator state

				targetBuffer[targetIndex].pos = sourceBuffer[i].pos;
				targetBuffer[targetIndex].velocity = sourceBuffer[i].velocity;
				targetBuffer[targetIndex].age = age;
				targetBuffer[targetIndex].isaGenerator = GL_TRUE;

				return 1;
			}
		}
		else {

			if (age < max_age) {

				targetBuffer[targetIndex].pos = sourceBuffer[i].pos + sourceBuffer[i].velocity * deltaTime;
				targetBuffer[targetIndex].velocity = sourceBuffer[i].velocity;
				targetBuffer[targetIndex].age = age;
				targetBuffer[targetIndex].isaGenerator = GL_FALSE;

				return 1;
			}
			else {

				// Particle dies - do not output to target buffer
				return 0;
			}
		}
	}

public:

	ParticleSystemCPU2(const GLuint maxParticles) {

		this->maxParticles = maxParticles;

		glCreateBuffers(2, buffer);
		
		glNamedBufferStorage(buffer[0], maxParticles * sizeof(BasicParticleGPU), 0, GL_MAP_WRITE_BIT);
		glNamedBufferStorage(buffer[1], maxParticles * sizeof(BasicParticleGPU), 0, GL_MAP_WRITE_BIT);

		currentBufferIndex = 0;

		// Map buffer and initialise generator
		auto initBuffer = (BasicParticleGPU*)glMapNamedBuffer(buffer[currentBufferIndex], GL_WRITE_ONLY);

		initBuffer[0] = BasicParticleGPU(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, true);

		glUnmapNamedBuffer(buffer[currentBufferIndex]);

		numLiveElements = 1;


		// Setup bindings for vertex shader input
		
		glCreateVertexArrays(1, &vao);

		glVertexArrayVertexBuffer(vao, particleBufferBindPoint, buffer[currentBufferIndex], 0, sizeof(BasicParticleGPU));
		
		glVertexArrayAttribBinding(vao, 0, particleBufferBindPoint);
		glVertexArrayAttribFormat(vao, 0, 3, GL_FLOAT, GL_FALSE, offsetof(BasicParticleGPU, pos));
		glEnableVertexArrayAttrib(vao, 0);

		glVertexArrayAttribBinding(vao, 1, particleBufferBindPoint);
		glVertexArrayAttribFormat(vao, 1, 3, GL_FLOAT, GL_FALSE, offsetof(BasicParticleGPU, velocity));
		glEnableVertexArrayAttrib(vao, 1);

		glVertexArrayAttribBinding(vao, 2, particleBufferBindPoint);
		glVertexArrayAttribFormat(vao, 2, 1, GL_FLOAT, GL_FALSE, offsetof(BasicParticleGPU, age));
		glEnableVertexArrayAttrib(vao, 2);

		//glVertexArrayAttribBinding(vao, 3, particleBufferBindPoint);
		//glVertexArrayAttribFormat(vao, 3, 4, GL_UNSIGNED_INT, GL_FALSE, offsetof(BasicParticleGPU, isaGenerator));
		//glEnableVertexArrayAttrib(vao, 3);
		
		
		// Initialise random number generation engine
		engine = new RandomEngine(-1.0f, 1.0f);

		// Load shaders
		renderShader = GLShaderFactory::CreateProgramObject(
			GLShaderFactory::CreateShader(GL_VERTEX_SHADER, std::string("shaders\\Particles\\basic_particle2.vs.txt")),
			GLShaderFactory::CreateShader(GL_GEOMETRY_SHADER, std::string("shaders\\Particles\\basic_particle2.gs.txt")),
			GLShaderFactory::CreateShader(GL_FRAGMENT_SHADER, std::string("shaders\\Particles\\basic_particle2.fs.txt"))
		);
	}


	void update(float deltaTime) {

		auto targetBufferIndex = 1 - currentBufferIndex;

		auto sourceBuffer = (BasicParticleGPU*)glMapNamedBuffer(buffer[currentBufferIndex], GL_WRITE_ONLY);
		auto targetBuffer = (BasicParticleGPU*)glMapNamedBuffer(buffer[targetBufferIndex], GL_WRITE_ONLY);

		// Update buffers

		GLuint targetCount = 0;

		for (GLuint i = 0; i < numLiveElements; ++i) {

			targetCount += updateParticle(sourceBuffer, i, targetBuffer, targetCount, maxParticles, engine, deltaTime);
		}

		numLiveElements = targetCount;

		// Unmap
		glUnmapNamedBuffer(buffer[currentBufferIndex]);
		glUnmapNamedBuffer(buffer[targetBufferIndex]);

		// Swap buffers and bind
		currentBufferIndex = targetBufferIndex;

		glVertexArrayVertexBuffer(vao, particleBufferBindPoint, buffer[currentBufferIndex], 0, sizeof(BasicParticleGPU));

	}

	void render(cst::ArcballCamera *camera) {

		GLuint shader = renderShader->useProgramObject();

		glm::mat4 viewTransform = camera->viewTransform();
		glm::mat4 projectionTransform = camera->projectionTransform();
		float nearPlaneDist = camera->getViewFrustum().nearPlaneDistance();
		float farPlaneDist = camera->getViewFrustum().farPlaneDistance();

		static GLint viewLocation = glGetUniformLocation(shader, "viewMatrix");
		static GLint projLocation = glGetUniformLocation(shader, "projMatrix");
		static GLint nearLocation = glGetUniformLocation(shader, "near");
		static GLint farLocation = glGetUniformLocation(shader, "far");
		static GLint maxAgeLocation = glGetUniformLocation(shader, "maxAge");

		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, (const GLfloat*)&viewTransform);
		glUniformMatrix4fv(projLocation, 1, GL_FALSE, (const GLfloat*)&projectionTransform);
		glUniform1f(nearLocation, nearPlaneDist);
		glUniform1f(farLocation, farPlaneDist);
		glUniform1f(maxAgeLocation, max_age);

		glBindVertexArray(vao);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glDrawArrays(GL_POINTS, 0, numLiveElements);
		
		glDisable(GL_BLEND);
	}
};
