#include "SDLApplication.h"



SDLApplication::SDLApplication(const char *windowTitle, int width, int height) {
	m_windowTitle = windowTitle; // set window title
	m_window = NULL;
	m_screenSurface = NULL;
	m_running = true;
	//SDL_Init(SDL_INIT_EVERYTHING);
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError());
		return;
	}
	m_window = SDL_CreateWindow(
		windowTitle,
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		width, height,
		SDL_WINDOW_SHOWN
	);
	if (m_window == NULL) {
		fprintf(stderr, "could not create window: %s\n", SDL_GetError());
		return;
	}
	m_screenSurface = SDL_GetWindowSurface(m_window);
}



void SDLApplication::pollEvents() {
	SDL_Event event;
	if (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			stop();
		}
	}
}


void SDLApplication::init() {
	SDL_Init(SDL_INIT_EVERYTHING);
}

void SDLApplication::clear() {
	SDL_FillRect(m_screenSurface, NULL, SDL_MapRGB(m_screenSurface->format, 120, 150, 200));
}

//void draw(const Mesh &mesh);

void SDLApplication::display() {
	SDL_UpdateWindowSurface(m_window);
}

void SDLApplication::terminate() {
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}


