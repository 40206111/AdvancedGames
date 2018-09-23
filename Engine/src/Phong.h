#pragma once
#include "Material.h"

class Phong: public Material{

public:
	// constructors
	Phong() {}

	// create Phong material from ambient, diffuse and specular colours and specular exponent n
	Phong(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, int n) {
		m_ambient = ambient;
		m_diffuse = diffuse;
		m_specular = specular;
		m_exponent = n;
		m_shader = ShaderProgram();
		m_shader.init();
		m_shader.use();
	}

	//accessors
	// get
	float getDiffuseWeight() { return m_ambientWeight; }
	glm::vec3 getDiffuse() { return m_diffuse; }
	int getEponent() { return m_exponent; }
	glm::vec3 getSpecular() { return m_specular; }
	float getSpecularWeight() { return m_specularWeight; }
	float getTransparency() { return m_transparency; }

	// set
	void setDiffuse(glm::vec3 diffuse) { m_diffuse = diffuse; }
	void setDiffuseWeight(float diffuseWeight) { m_diffuseWeight = diffuseWeight; }
	void setExponent(int n) { m_exponent = n; }
	void setSpecular(glm::vec3 specular) { m_specular = specular; }
	void setSpecularWeight(float specularWeight) { m_specularWeight = specularWeight; }
	void setTransparency(float transparency) { m_transparency = transparency; }

private:

	// diffuse
	glm::vec3 m_diffuse;
	float m_diffuseWeight;
	Texture2D m_diffuseTexture;

	// specular
	glm::vec3 m_specular;
	float m_specularWeight;
	float m_transparency;
	Texture2D m_specularTexture;
	int m_exponent; // exponent of the Phong model


};