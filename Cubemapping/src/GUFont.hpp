#pragma once

class GUFont {

	GLuint				fnt_base;

public:

	GUFont(const std::wstring& fontName, const int fontSize);
	~GUFont();

	void renderText(const float x, const float y, const glm::mat4& view, const glm::vec4& colour, const char *formatString, ...) const;

private:

	void deleteFont();

};
