#pragma once

#include "StringUtility.hpp"

class GLShaderStage_actual {

	GLuint			shader;

#pragma region Private API

	void printSourceListing(std::string& sourceString, bool showLineNumbers = true) {

		const char* srcPtr = sourceString.c_str();
		const char* srcEnd = srcPtr + sourceString.length();

		size_t lineIndex = 0;

		while (srcPtr < srcEnd) {

			if (showLineNumbers) {

				std::cout.fill(' ');
				std::cout.width(4);
				std::cout << std::dec << ++lineIndex << " > ";
			}

			size_t substrLength = strcspn(srcPtr, "\n");
			std::cout << std::string(srcPtr, 0, substrLength) << std::endl;
			srcPtr += substrLength + 1;
		}
	}

	void reportShaderInfoLog(GLuint shader) {

		GLsizei noofBytes = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &noofBytes);

		GLchar* str = (GLchar*)calloc(noofBytes + 1, 1);

		if (str) {

			glGetShaderInfoLog(shader, noofBytes, 0, str);
			std::cout << str << std::endl;
			free(str);
		}
	}

#pragma endregion

public:

	enum class ShaderError : uint8_t {

		GLSL_OKAY = 0,
		GLSL_SHADER_SOURCE_NOT_FOUND,
		GLSL_SHADER_OBJECT_CREATION_ERROR,
		GLSL_SHADER_COMPILE_ERROR
	};

	GLShaderStage_actual(GLenum shaderType, const std::string& shaderPath) {

		std::string sourceString;

		try {

			shader = 0;
			sourceString = StringUtility::loadStringFromFile(shaderPath);

			shader = glCreateShader(shaderType);

			if (shader == 0)
				throw GLShaderStage_actual::ShaderError::GLSL_SHADER_OBJECT_CREATION_ERROR;

			const char* src = sourceString.c_str();
			glShaderSource(shader, 1, static_cast<const GLchar**>(&src), 0);

			glCompileShader(shader);

			GLint compileStatus;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);

			if (compileStatus == 0)
				throw GLShaderStage_actual::ShaderError::GLSL_SHADER_COMPILE_ERROR;
		}
		catch (StringUtility::StringResult err) {

			std::set<char> pathDelimiters{ '\\' };
			std::vector<std::string> pathComponents = StringUtility::splitPath(shaderPath, pathDelimiters);

			if (err == StringUtility::StringResult::S_FILE_NOT_FOUND) {

				std::cout << (pathComponents[pathComponents.size() - 1]) << " source not found. Check the file path in your code.\n";
			}
			else if (err == StringUtility::StringResult::S_BUFFER_ALLOC_ERROR) {

				std::cout << (pathComponents[pathComponents.size() - 1]) << " string buffer could not be created. Try freeing up resources before attempting to create the shader.\n";
			}
		}
		catch (GLShaderStage_actual::ShaderError err) {

			std::set<char> pathDelimiters{ '\\' };
			std::vector<std::string> pathComponents = StringUtility::splitPath(shaderPath, pathDelimiters);

			if (err == GLShaderStage_actual::ShaderError::GLSL_SHADER_OBJECT_CREATION_ERROR) {

				std::cout << (pathComponents[pathComponents.size() - 1]) << " shader object could not be created.  Try freeing up resources before attempting to create the shader.\n";
			}
			else if (err == GLShaderStage_actual::ShaderError::GLSL_SHADER_COMPILE_ERROR) {

				std::cout << pathComponents[pathComponents.size() - 1] << " could not be compiled successfully...\n\n";
				printSourceListing(sourceString);

				// Report compilation error log
				std::cout << "\n<" << pathComponents[pathComponents.size() - 1] << " shader compiler errors--------------------->\n\n";
				reportShaderInfoLog(shader);
				std::cout << "<-----------------end " << pathComponents[pathComponents.size() - 1] << " shader compiler errors>\n\n\n";

				glDeleteShader(shader);
				shader = 0;
			}
		}
	}

	~GLShaderStage_actual() {

		if (shader != 0)
			glDeleteShader(shader);
	}

	GLuint getShader() const {

		return shader;
	}
};


// Alias to shared pointer
using GLShaderStage = std::shared_ptr<GLShaderStage_actual>;
