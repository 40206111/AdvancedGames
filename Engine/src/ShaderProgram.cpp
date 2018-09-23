#include "ShaderProgram.h"

#include <fstream>
#include <iostream>
#include <sstream>

ShaderProgram::ShaderProgram(): m_handle(0) {

}

ShaderProgram::~ShaderProgram() {
	glDeleteProgram(m_handle);
}


bool ShaderProgram::loadShaders(const char* vsFilename, const char* fsFilename) {

	string vsString = fileToString(vsFilename);
	string fsString = fileToString(fsFilename);
	
	const GLchar* vsSourcePtr = vsString.c_str();
	const GLchar* fsSourcePtr = fsString.c_str();

	// vertex shader source
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vsSourcePtr, NULL);
	glCompileShader(vs);
	
	// check for compilation errors
	checkCompileErrors(vs, VERTEX);
	
	// fragment shader source
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fsSourcePtr, NULL);
	glCompileShader(fs);
	
	// check for compilation errors
	checkCompileErrors(fs, FRAGMENT);
	

	// create shader program
	m_handle = glCreateProgram();
	glAttachShader(m_handle, vs);
	glAttachShader(m_handle, fs);
	glLinkProgram(m_handle);
	
	checkCompileErrors(m_handle, PROGRAM);

	// the shaders are linked to a program, so the shaders themselves are no longer required.
	glDeleteShader(vs);
	glDeleteShader(fs);

	return true;

}


void ShaderProgram::use() {
	if (m_handle > 0) {
		glUseProgram(m_handle);
	}

}

void ShaderProgram::init() {
	loadShaders("./resources/shaders/default.vert", "./resources/shaders/default.frag");
}

string ShaderProgram::fileToString(const string& filename) {
	std::stringstream ss;
	std::ifstream file;

	std::cout << "filename " << filename << std::endl;


	try {
		file.open(filename, std::ios::in);
		if (!file.fail()) {
			ss << file.rdbuf();
		}
		file.close();
	}
	catch (std::exception ex) {
		std::cerr << " Error reading shader file: " << filename << std::endl;
	}

	return ss.str();

}

void ShaderProgram::checkCompileErrors(GLuint shader, ShaderType type) {
	int status = 0;

	if (type == PROGRAM) {
		glGetProgramiv(m_handle, GL_LINK_STATUS, &status);
		if (status == GL_FALSE) {
			GLint length;
			glGetProgramiv(m_handle, GL_INFO_LOG_LENGTH, &length);

			string errorLog(length, ' ');
			glGetProgramInfoLog(m_handle, length, &length, &errorLog[0]);
			std::cerr << "Error! Program failed to link. " << errorLog << std::endl;
		}
	}
	else { // VERTEX or FRAGMENT
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE) {
			GLint length;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

			string errorLog(length, ' ');
			glGetShaderInfoLog(shader, length, &length, &errorLog[0]);
			std::cerr << "Error! Shader failed to compile. (" << type << "): " << errorLog << std::endl;
		}
	}
}
