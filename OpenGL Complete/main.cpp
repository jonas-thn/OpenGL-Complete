#include <iostream>

#include <GL/glew.h>

#define SDL_MAIN_HANDLED
#include <SDL.h>

SDL_Window* window;
bool close = false;

void init()
{
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	window = SDL_CreateWindow("OpenGL Complete", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);

	SDL_GLContext glContext = SDL_GL_CreateContext(window);

	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		std::cout << "Error: " << glewGetErrorString(err) << std::endl;
		exit(-1);
	}

	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
}

void process_input()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			close = true;
		}
	}
}
void update()
{

}

void render()
{
	glClearColor(1.0f, 0.85f, 0.6f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	SDL_GL_SwapWindow(window);
}

int main(int argc, char* argv[])
{
	init();

	while (!close)
	{
		update();
		render();
	}

	return 0;
}