#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>

#define SDL_MAIN_HANDLED
#include <SDL.h>

#include "Shader.h"

const int WIDTH = 800;
const int HEIGHT = 600;

SDL_Window* window;
SDL_GLContext glContext;
bool close = false;

float vertices[] = {
	-0.5f, -0.5f, 0.0f,
	0.5f, -0.5f, 0.0f,
	0.0f, 0.5f, 0.0f
};

unsigned int VBO;
unsigned int VAO;

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

	if (window == NULL)
	{
		std::cout << "Window Creation Error" << std::endl;
		exit(-1);
	}

	glContext = SDL_GL_CreateContext(window);

	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		std::cout << "Error Glew Init: " << glewGetErrorString(err) << std::endl;
		exit(-1);
	}

	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;

	glViewport(0, 0, WIDTH, HEIGHT);
}

void setup()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	Shader shader("vertex.vert", "fragment.frag");

	shader.UseShader();
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

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	SDL_GL_SwapWindow(window);
}

void cleanup()
{
	SDL_DestroyWindow(window);
	SDL_GL_DeleteContext(glContext);
	SDL_Quit();
}

int main(int argc, char* argv[])
{
	init();
	setup();

	while (!close)
	{
		update();
		render();
	}

	cleanup();

	return 0;
}