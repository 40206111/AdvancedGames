#pragma once

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>

using std::string;

class ShaderProgram {

public:
	ShaderProgram();
	~ShaderProgram();

	enum ShaderType{
		VERTEX,
		FRAGMENT,
		PROGRAM
	};

	GLuint getHandle() { return m_handle; }

	bool loadShaders(std::string vsFile, std::string fsFile) { return loadShaders(vsFile.c_str(), fsFile.c_str()); }
	bool loadShaders(const char* vsFile, const char* fsFile);
	void init();
	void use();


private:

	string fileToString(const string& filename);
	void checkCompileErrors(GLuint shader, ShaderType type);

	GLuint m_handle;

};