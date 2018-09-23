#pragma once

#include <string>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

using std::string;

class Texture2D {
public:
	Texture2D();
	virtual ~Texture2D();

	// accessors
	GLuint getTexture() { return m_texture; }

	// functions
	bool loadTexture(const string& filename, bool generateMipMaps = true);
	void bind(GLuint var= 0);

private:
	GLuint m_texture;
};