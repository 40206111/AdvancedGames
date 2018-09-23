#pragma once
// GLM
#include <glm/glm.hpp>
#include "glm/ext.hpp"

#include "Texture2D.h"
#include "ShaderProgram.h"

class Material {

public:
	// accessors
	glm::vec3 getAmbient() { return m_ambient; }
	ShaderProgram getShader() { return m_shader; }
	
	void setAmbient(glm::vec3 ambient) { m_ambient = ambient; }
	void setambientWeight(float ambientWeight) { m_ambientWeight = ambientWeight; }

protected:
	// material properties
	
	// ambient
	glm::vec3 m_ambient;
	float m_ambientWeight;
	Texture2D m_ambientTexture;
	
	// shaders
	ShaderProgram m_shader;
	string m_shaderSrc;
};