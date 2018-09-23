#pragma once
#include "Application.h"
#include "Transform.h"

class Geometry {

public:
	Transform getTransform() const { return m_transform; }

	// translate mesh by a vector
	void translate(const glm::vec3 &vect) { m_transform.translate(vect); }
	// rotate mesh by a vector
	void rotate(const float &angle, const glm::vec3 &vect) { m_transform.rotate(angle, vect); }
	// scale mesh by a vector
	void scale(const glm::vec3 &vect) { m_transform.scale(vect); }

	// draw geometry
	virtual void bindGeometry() { std::cout << "Can't bind generic geometry. can only be implemented for Polymesh objects." << std::endl; }
	virtual void draw(const Application &app) { std::cout << "Can't draw object of class Geometry. draw only implemented for Polymesh objects." << std::endl; }

private:
	// transform
	Transform m_transform;
};