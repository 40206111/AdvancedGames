#include <iostream>
#include <SDL.h>

#include "Rasterizer.h";

#include "../Application.h"
#include "../Model.h"


void Rasterizer::draw(const Model &model, const Application &app) {

	model.getMaterial().getShader().use();
	//texture.bind();
	model.getGeometry()->bindGeometry();
	glUseProgram(model.getMaterial().getShader().getHandle());

	
	// Get the uniform locations for MVP
	GLint modelLoc = glGetUniformLocation(model.getMaterial().getShader().getHandle(), "model");
	GLint viewLoc = glGetUniformLocation(model.getMaterial().getShader().getHandle(), "view");
	GLint projLoc = glGetUniformLocation(model.getMaterial().getShader().getHandle(), "projection");
	GLint rotateLoc = glGetUniformLocation(model.getMaterial().getShader().getHandle(), "rotate");


	// Pass the matrices to the shader
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr((model.getGeometry()->getTransform().getModel())));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(app.getView()));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(app.getProjection()));
	glUniformMatrix4fv(rotateLoc, 1, GL_FALSE, glm::value_ptr((model.getGeometry()->getTransform().getRotate())));

	model.getGeometry()->draw(app);
}



