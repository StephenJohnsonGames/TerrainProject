
#include <pch.h>
#include "GUFont.hpp"


using namespace std;


GUFont::GUFont(const wstring& fontName, const int fontSize) {

	fnt_base = 0;

	// Get device context
	HDC hDC = GetDC(NULL);

	if (!hDC)
		return;

	// Generate and validate display list
	fnt_base = glGenLists(256);

	if (!fnt_base)
		return;

	HFONT font = CreateFont(-fontSize,				// Height Of Font
		0,							// Width Of Font
		0,							// Angle Of Escapement
		0,							// Orientation Angle
		FW_BOLD,					// Font Weight
		FALSE,						// Italic
		FALSE,						// Underline
		FALSE,						// Strikeout
		ANSI_CHARSET,				// Character Set Identifier
		OUT_TT_PRECIS,				// Output Precision
		CLIP_DEFAULT_PRECIS,		// Clipping Precision
		ANTIALIASED_QUALITY,		// Output Quality
		FF_DONTCARE | DEFAULT_PITCH,	// Family And Pitch
		fontName.c_str());

	if (!font) {

		deleteFont();
		ReleaseDC(NULL, hDC);
		return;
	}

	// select new font
	SelectObject(hDC, font);

	// setup display lists to render font (using ASCII char set)
	wglUseFontBitmaps(hDC, 0, 256, fnt_base);

	// release device context and dispose local resources
	ReleaseDC(NULL, hDC);
	DeleteObject(font);
}


GUFont::~GUFont() {

	deleteFont();
}


void GUFont::renderText(const float x, const float y, const glm::mat4& view, const glm::vec4& colour, const char *formatString, ...) const {

	if (!formatString)
		return;

	// Parse the string for variables
	const int	maxStringLength = 256;

	char		text[maxStringLength];
	va_list		ap;

	va_start(ap, formatString);
	vsprintf_s(text, maxStringLength, formatString, ap);
	va_end(ap);

	// Render text

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMultMatrixf((GLfloat*)&(view));

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glRasterPos2f(x, y);

	glColor4f(colour.r, colour.g, colour.b, colour.a);

	glPushAttrib(GL_LIST_BIT);

	glListBase(fnt_base);
	glCallLists((GLsizei)strlen(text), GL_UNSIGNED_BYTE, text);

	glPopAttrib();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}



// Private method implementation

void GUFont::deleteFont() {

	if (fnt_base)
		glDeleteLists(fnt_base, 256);

	fnt_base = 0;
}
