#pragma once

// GLM
#include <glm/glm.hpp>
#include "glm/ext.hpp"

class Transform
{
public:
	Transform() { init(); }
	~Transform() {}

	/*
		ACCESSORS
	*/
	glm::mat4 getTranslate() { return m_translate; }
	glm::mat4 getRotate() { return m_rotate; }
	glm::mat4 getScale() { return m_scale; }

	/*
		INITIALISATION
	*/
	void init() {
		m_translate = glm::mat4(1.0f);
		m_rotate = glm::mat4(1.0f);
		m_scale = glm::mat4(1.0f);
	}

	/*
		TRANSFORMATION METHODS
	*/
	// translate mesh by a vector
	void translate(const glm::vec3 &vect) { m_translate = glm::translate(m_translate, vect); }
	// rotate mesh by a vector
	void rotate(const float &angle, const glm::vec3 &vect) { m_rotate = glm::rotate(m_rotate, angle, vect); }
	// scale mesh by a vector
	void scale(const glm::vec3 &vect) { m_scale = glm::scale(m_scale, vect); }


	/*
		OTHER USEFUL METHODS
		Transformations are expected to be carried out using the above transformation methods, but the methods below can also be useful
	*/

	// get position of object
	glm::vec3 getPos() { return getTranslate()[3]; }

	// get model matrix (computed at every call)
	glm::mat4 getModel() { return getTranslate() * getRotate() * getScale(); }

	// set position specified 3D position vector
	void setPos(const glm::vec3 &position) {
		m_translate[3][0] = position[0];
		m_translate[3][1] = position[1];
		m_translate[3][2] = position[2];
	}
	// set i_th coordinate to float p (x: i=0, y: i=1, z: i=2)
	void setPos(int i, float p) { m_translate[3][i] = p; }

	// set rotation matrix
	void setRotate(const glm::mat4 &mat) { m_rotate = mat; }

private:
	glm::mat4 m_translate;
	glm::mat4 m_rotate;
	glm::mat4 m_scale;
};
