#pragma once
#include <iostream>

// GLM
#include <glm/glm.hpp>
#include "glm/ext.hpp"

#include "Texture2D.h"
#include "ShaderProgram.h"


class Application
{
public:

	// accessors
	virtual bool shouldRun() = 0;

	// application window size
	GLuint getWidth() { return m_width; }
	GLuint getHeight() { return m_height; }

	// camera information
	glm::mat4 getView() const { return m_view; }
	void setView(const glm::mat4 &view) { m_view = view; }
	glm::mat4 getProjection() const { return m_projection; }
	void setProjection(const glm::mat4 &projection) { m_projection = projection; }

	/*
		Framework specific methods
	*/
	// rendering
	virtual void init()=0; //initialises render
	virtual void clear()=0; // clear buffer
	virtual void display()=0; // display buffer on window
	virtual void terminate()=0; // closes application

	// event management
	virtual void pollEvents()=0;

	// other functions
	virtual void showFPS()=0;

	/*
		Rendering methods
	*/
	//void draw(const Mesh &mesh); // draw model
	//void draw(const Mesh &mesh, Texture2D texture); // draw model
	//void draw(const PolyMesh &mesh, ShaderProgram shader, Texture2D texture); // draw model
	


protected:
	// window property
	const char *m_windowTitle;
	GLuint m_width;
	GLuint m_height;
	int m_bufferWidth = 0;
	int m_bufferHeight = 0;

	// view and projection matrices
	glm::mat4 m_view = glm::mat4(1.0f);
	glm::mat4 m_projection = glm::mat4(1.0f);

private:
	
};

