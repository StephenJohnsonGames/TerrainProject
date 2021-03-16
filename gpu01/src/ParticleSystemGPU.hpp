#pragma once

// Create a GPU-based particle system - no CPU intervention or access is needed here.
// The system is mangaged in two phases: update and draw.  Update uses transform feedback to write particles to a buffer rather than relying on the CPU to do this as in the previous examples.
// The draw phase is the same as before, using eye-space billboarding to generate the geometry in the geometry shader.


#include "ArcballCamera.hpp"
#include "RandomEngine.hpp"
#include "GLShaderFactory.hpp"
#include "BasicParticleGPU.hpp"


class ParticleSystemGPU {

	GLuint							maxParticles;

	GLuint							buffer[2]; // Particle buffer objects
	GLuint							xfb[2]; // Transform feedback buffer objects

	GLuint							vao;

	GLuint							currentBufferIndex;
	GLuint							numLiveElements;

	RandomEngine*					engine = nullptr;
	GLuint							randomArraySSO; //shader storage object

	const GLfloat					max_age = 8.0f;
	const GLfloat					generateAge = 0.02f; // frequency of particle generation (time interval between generation events)

	const GLuint					particleBufferBindPoint = 0;

	GLProgramObject					updateShader;
	GLProgramObject					renderShader;

	GLuint							texBuffer;
	GLuint							snowTexture;



	// Private API

	void testBuffer(GLuint bufferToTest) {

		auto testBuffer = (BasicParticleGPU*)glMapNamedBuffer(bufferToTest, GL_READ_ONLY);

		// test
		for (GLuint i = 0; i < maxParticles; ++i) {

			std::cout << "[" << i << "]: ";
			testBuffer[i].print();
		}

		glUnmapNamedBuffer(bufferToTest);
	}


	void update(float deltaTime) {

		static bool isFirstUpdate = true;

		glEnable(GL_RASTERIZER_DISCARD); //cuts off at the geo shader
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, xfb[currentBufferIndex]);
		
		GLuint shader = updateShader->useProgramObject();

		static GLint maxAgeLocation = glGetUniformLocation(shader, "maxAge");
		static GLint deltaTimeLocation = glGetUniformLocation(shader, "deltaTime");
		static GLint genAgeLocation = glGetUniformLocation(shader, "generateAge");
		static GLint rndLocation = glGetUniformLocation(shader, "rndBaseIndex");
		
		GLint r = GLint(engine->getRandomValue() * 1023.0f);
		glUniform1i(rndLocation, r);

		glUniform1f(maxAgeLocation, max_age);
		glUniform1f(deltaTimeLocation, deltaTime);
		glUniform1f(genAgeLocation, generateAge);

		// Push particle buffer through transform feedback
		glBindVertexArray(vao);

		glBeginTransformFeedback(GL_POINTS); //send to transform feedback buffer sequence

		if (isFirstUpdate) {

			// If very first update push through known number of live particle elements set in the constructor
			glDrawArrays(GL_POINTS, 0, numLiveElements);
		}
		else {

			// If not the first pass, use the (internally) recorded number of particles in the buffer stored in the transform feedback object.
			glDrawTransformFeedback(GL_POINTS, xfb[1 - currentBufferIndex]);
		}
		
		glFlush();

		glEndTransformFeedback();
		
		glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
		glDisable(GL_RASTERIZER_DISCARD);
		
		// Swap buffers and bind
		currentBufferIndex = 1 - currentBufferIndex;
		glVertexArrayVertexBuffer(vao, particleBufferBindPoint, buffer[currentBufferIndex], 0, sizeof(BasicParticleGPU));

		// Subsequent calls will use the internal transform feedback vertex / particle count
		isFirstUpdate = false;
	}

	void draw(cst::ArcballCamera* camera) {

		GLuint shader = renderShader->useProgramObject();

		glm::mat4 viewTransform = camera->viewTransform();
		glm::mat4 projectionTransform = camera->projectionTransform();
		float nearPlaneDist = camera->getViewFrustum().nearPlaneDistance();
		float farPlaneDist = camera->getViewFrustum().farPlaneDistance();
		glm::mat4 Tr = glm::translate(glm::mat4(1.0f), glm::vec3(3.0f, 0.0f, 3.0f));

		static GLint viewLocation = glGetUniformLocation(shader, "viewMatrix");
		static GLint projLocation = glGetUniformLocation(shader, "projMatrix");
		static GLint nearLocation = glGetUniformLocation(shader, "near");
		static GLint farLocation = glGetUniformLocation(shader, "far");
		static GLint maxAgeLocation = glGetUniformLocation(shader, "maxAge");
		static GLint translate = glGetUniformLocation(shader, "translate");
		
		glUniformMatrix4fv(viewLocation, 1, GL_FALSE, (const GLfloat*)&viewTransform);
		glUniformMatrix4fv(projLocation, 1, GL_FALSE, (const GLfloat*)&projectionTransform);
		glUniform1f(nearLocation, nearPlaneDist);
		glUniform1f(farLocation, farPlaneDist);
		glUniform1f(maxAgeLocation, max_age);
		glUniformMatrix4fv(translate, 1, GL_FALSE, (const GLfloat*)&Tr);

		glBindVertexArray(vao);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Since draw ALWAYS follows update, we use the transform feedback (internal) vertex counts
		glDrawTransformFeedback(GL_POINTS, xfb[1 - currentBufferIndex]);

		glDisable(GL_BLEND);
	}

public:

	ParticleSystemGPU(const GLuint maxParticles) {

		glm::vec2* texArray = (glm::vec2*)malloc(maxParticles * sizeof(glm::vec2));

		this->maxParticles = maxParticles;

		// Create initial particle buffers
		glCreateBuffers(2, buffer);

		glNamedBufferStorage(buffer[0], maxParticles * sizeof(BasicParticleGPU), 0, GL_MAP_WRITE_BIT | GL_MAP_READ_BIT);
		glNamedBufferStorage(buffer[1], maxParticles * sizeof(BasicParticleGPU), 0, GL_MAP_WRITE_BIT | GL_MAP_READ_BIT);

		currentBufferIndex = 0;

		// Map buffer and initialise generator
		auto initBuffer0 = (BasicParticleGPU*)glMapNamedBuffer(buffer[0], GL_WRITE_ONLY);

		initBuffer0[0] = BasicParticleGPU(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, GL_TRUE);
		
		glUnmapNamedBuffer(buffer[0]);

		numLiveElements = 1;

		// -----------

		// Setup transform feedback objects
		glCreateTransformFeedbacks(2, xfb);

		glTransformFeedbackBufferBase(xfb[0], 0, buffer[1]); // When current buffer is 0, xfb writes to buffer[1]
		glTransformFeedbackBufferBase(xfb[1], 0, buffer[0]); // When current buffer is 1, xfb writes to buffer[0]

		// -----------

		// Setup bindings for vertex shader input (both update and rendering see the same data)

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

		glVertexArrayAttribBinding(vao, 3, particleBufferBindPoint);
		glVertexArrayAttribIFormat(vao, 3, 1, GL_INT, offsetof(BasicParticleGPU, isaGenerator));
		glEnableVertexArrayAttrib(vao, 3);

		//texture
		glCreateBuffers(1, &texBuffer);
		glNamedBufferStorage(texBuffer, maxParticles * sizeof(glm::vec2), texArray, 0);
		glVertexArrayVertexBuffer(vao, 5, texBuffer, 0, sizeof(glm::vec2));
		glVertexArrayAttribBinding(vao, 5, 5);
		glVertexArrayAttribFormat(vao, 5, 5, GL_FLOAT, GL_FALSE, 0);
		glEnableVertexArrayAttrib(vao, 5);

		// ------

		// GLSL does not implement random numbers so we'll provide a table and pass as a shader storage object (not uniform buffer since we get std430 alignment which is more efficient)
		// Initialise random number generation engine - store random values in range 0 to 1
		engine = new RandomEngine(0.0f, 1.0f);

		const GLuint rndSize = 1024;
		GLuint numBytes = rndSize * sizeof(float);
		float* randomBuffer = (float*)malloc(numBytes);
		for (GLuint i = 0; i < rndSize; ++i)
			randomBuffer[i] = engine->getRandomValue();

		// Setup SSO
		glGenBuffers(1, &randomArraySSO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, randomArraySSO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, numBytes, randomBuffer, GL_STATIC_READ);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, randomArraySSO);

		// ------

		snowTexture = fiLoadTexture("..\\shared_resources\\Textures\\Terrain\\rock.jpg", TextureProperties(true));

		// Load shaders

		updateShader = GLShaderFactory::CreateProgramObject(
			GLShaderFactory::CreateShader(GL_VERTEX_SHADER, std::string("shaders\\Particles\\gpu_particle_update.vs.txt")),
			GLShaderFactory::CreateShader(GL_GEOMETRY_SHADER, std::string("shaders\\Particles\\gpu_particle_update.gs.txt"))
		);

		renderShader = GLShaderFactory::CreateProgramObject(
			GLShaderFactory::CreateShader(GL_VERTEX_SHADER, std::string("shaders\\Particles\\gpu_particle_draw.vs.txt")),
			GLShaderFactory::CreateShader(GL_GEOMETRY_SHADER, std::string("shaders\\Particles\\gpu_particle_draw.gs.txt")),
			GLShaderFactory::CreateShader(GL_FRAGMENT_SHADER, std::string("shaders\\Particles\\gpu_particle_draw.fs.txt"))
		);
	}


	void render(cst::ArcballCamera *camera, float deltaTime) {

		// Pass 1 - update particles
		update(deltaTime);

		// Pass 2 - render particles
		draw(camera);
	}
};
