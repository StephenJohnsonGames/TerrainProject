#pragma once

#include "GLShaderStage.hpp"
#include "GLProgramObject.hpp"

class GLShaderFactory {

public:

	static GLShaderStage CreateShader(GLenum shaderType, const std::string& shaderPath) {

		return std::make_shared<GLShaderStage_actual>(shaderType, shaderPath);
	}

	template <class... Args>
	static GLProgramObject CreateProgramObject(const Args... args) {

		return std::make_shared<GLProgramObject_actual>(args...);
	}
};

