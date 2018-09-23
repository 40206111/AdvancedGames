#pragma once
#include "Application.h"

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>
// SDL
#include <SDL.h>

class SDLApplication : public Application{

public:
	// constructors
	SDLApplication() {}
	SDLApplication(const char *windowTitle, int width, int height);
	~SDLApplication() {}

	// accessors
	bool isRunning() { return m_running; }

	// render methods
	void init(); //initialises render
	void clear(); // clear buffer
						  //void draw(const Mesh &mesh); // draw mesh
	void display(); // display buffer on window
	void terminate(); // closes application

	// event management
	void pollEvents();

	// other methods
	void showFPS() {}

private:
	SDL_Window* m_window;
	SDL_Surface* m_screenSurface;
};

