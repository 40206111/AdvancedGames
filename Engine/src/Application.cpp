#include <iostream>
#include <SDL.h>

#include "Application.h"

//void Application::draw(const PolyMesh &mesh, ShaderProgram shader, Texture2D texture) {
//
//	shader.use();
//	texture.bind();
//
//	glBindVertexArray(mesh.getVAO());
//	glUseProgram(shader.getHandle());
//
//	// Get the uniform locations for MVP
//	GLint modelLoc = glGetUniformLocation(shader.getHandle(), "model");
//	GLint viewLoc = glGetUniformLocation(shader.getHandle(), "view");
//	GLint projLoc = glGetUniformLocation(shader.getHandle(), "projection");
//	GLint rotateLoc = glGetUniformLocation(shader.getHandle(), "rotate");
//
//
//	// Pass the matrices to the shader
//	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(dynamic_cast<const Geometry&>(mesh).getTransform().getModel()));
//	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(m_view));
//	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(m_projection));
//	glUniformMatrix4fv(rotateLoc, 1, GL_FALSE, glm::value_ptr(dynamic_cast<const Geometry&>(mesh).getTransform().getRotate()));
//
//
//	// draw geometry
//	switch (mesh.getCreationMode()) {
//	case PolyMesh::SIMPLE:
//		//std::cout << "drawing, simple mode" << std::endl;
//		glDrawArrays(GL_TRIANGLES, 0, mesh.getVertexNum());
//		break;
//	case PolyMesh::INDEXED:
//		//std::cout << "drawing, indexed mode" << std::endl;
//		glDrawElements(GL_TRIANGLES, mesh.getVertexNum(), GL_UNSIGNED_INT, 0);
//		break;
//	}
//	glBindVertexArray(0);
//}

