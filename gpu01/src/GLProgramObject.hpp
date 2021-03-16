#pragma once

#include "GLShaderStage.hpp"

class GLProgramObject_actual {

	GLuint		program;

#pragma region Private API

	void initialiseStages(std::vector<GLShaderStage>& pipelineStages) {}

	template <class... Args>
	void initialiseStages(std::vector<GLShaderStage>& pipelineStages, GLShaderStage s1, Args... args) {

		pipelineStages.push_back(s1);
		initialiseStages(pipelineStages, args...);
	}

	void reportProgramInfoLog()
	{
		GLsizei noofBytes = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &noofBytes);

		GLchar* str = (GLchar*)calloc(noofBytes + 1, 1);

		if (str) {

			glGetProgramInfoLog(program, noofBytes, 0, str);
			std::cout << str << std::endl;
			free(str);
		}
	}

#pragma endregion

public:

	template <class... Args>
	GLProgramObject_actual(const Args... args) {

		std::vector<GLShaderStage> pipelineStages;

		// Create vector of pipeline stages
		initialiseStages(pipelineStages, args...);

		// Once shader objects have been setup, setup the main shader program object
		program = glCreateProgram();

		if (program == 0) {

			std::cout << "The shader program object could not be created.\n";
			return;
		}

		// Attach shaders
		for (std::vector<GLShaderStage>::iterator i = pipelineStages.begin(); i != pipelineStages.end(); ++i)
			glAttachShader(program, (*i)->getShader());

		// Link and validate the shader program
		glLinkProgram(program);

		GLint linkStatus;
		glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

		if (linkStatus == 0) {

			// Failed to link - report linker error log and dispose of local resources

			std::cout << "The shader program object could not be linked successfully...\n";

			std::cout << "\n<GLSL shader program object linker errors--------------------->\n\n";
			reportProgramInfoLog();
			std::cout << "<-----------------end shader program object linker errors>\n\n";

			glDeleteProgram(program);
			program = 0;
		}
	}


	GLuint getProgramObject() {

		return program;
	}

	GLuint useProgramObject() {

		glUseProgram(program);

		return program;
	}
};

using GLProgramObject = std::shared_ptr<GLProgramObject_actual>;
