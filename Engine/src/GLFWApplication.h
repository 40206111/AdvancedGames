#pragma once

#include "Application.h"

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>



class GLFWApplication : public Application {

	static int SCREEN_WIDTH, SCREEN_HEIGHT;

public:
	// constructors
	GLFWApplication() {}
	GLFWApplication(const char *windowTitle, int width, int height);
	~GLFWApplication() {}

	// render methods
	void init() override; //initialises render
	void clear() override; // clear buffer
	void display() override; // display buffer on window
	void terminate() override; // closes application

	// event management
	void pollEvents();

	// other methods
	bool shouldRun();
	void showFPS();
	GLFWwindow* getWindow() { return m_window; }


private:
	GLFWwindow* m_window = NULL;
};