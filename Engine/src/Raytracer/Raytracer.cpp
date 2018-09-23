#include <iostream>
#include <SDL.h>

#include "Raytracer.h";



void Raytracer::draw(const Scene &scene) {

	m_shader.use();
	glBindVertexArray(m_vao);
	glUseProgram(m_shader.getHandle());
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

}



