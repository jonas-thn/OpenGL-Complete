#include <iostream>
#include <memory>

#define GLEW_STATIC
#include <GL/glew.h>

#define SDL_MAIN_HANDLED
#include <SDL.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Input.h"
#include "Camera.h"

const int WIDTH = 800;
const int HEIGHT = 600;

SDL_Window* window;
SDL_GLContext glContext;
bool close = false;

float vertices[] = {
0.5f, 0.5f, 0.0f, 1.0f, 1.0f, // top right
0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // bottom right
-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
-0.5f, 0.5f, 0.0f, 0.0f, 1.0f // top left
};

unsigned int indices[] = { 
0, 1, 3, // first triangle
1, 2, 3 // second triangle
};

unsigned int VBO;
unsigned int VAO;
unsigned int EBO;

//std::unique_ptr<Shader> shader = nullptr;
//std::unique_ptr<Input> input = nullptr;

Shader* shader = nullptr;
Input* input = nullptr;
Camera* camera = nullptr;

unsigned int texture;

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
	glEnable(GL_DEPTH_TEST);
}

void setup()
{
	//----------VBO, VAO, EBO----------

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);

	//----------INIT----------

	/*shader = std::make_unique<Shader>("vertex.vert", "fragment.frag");
	input = std::make_unique<Input>();*/

	shader = new Shader("vertex.vert", "fragment.frag");
	input = new Input();
	camera = new Camera(WIDTH, HEIGHT, *input);

	//----------TEXTURE----------

	stbi_set_flip_vertically_on_load(true);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	
	int width, height, nrChannels;
	unsigned char* data = stbi_load("Textures/container.jpg", &width, &height, &nrChannels, 0);

	if (data)
	{
		if (nrChannels == 3)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else if (nrChannels == 4)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
	}
	else
	{
		std::cout << "Texture ERROR" << std::endl;
	}

	stbi_image_free(data);

	//----------TRANFORM----------


}

void process_input()
{
	input->HandleInput();
	close = input->GetQuit();
}
void update(float deltaTime)
{
	camera->UpdateView(deltaTime);
}

void render()
{
	glClearColor(1.0f, 0.85f, 0.6f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader->UseShader();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	shader->SetInt("texture0", 0);

	glm::mat4 model = glm::mat4(1.0f);

	shader->SetMat4("model", model);
	shader->SetMat4("view", camera->GetView());
	shader->SetMat4("proj", camera->GetProj());

	glBindVertexArray(VAO);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	SDL_GL_SwapWindow(window);
}

void cleanup()
{
	SDL_DestroyWindow(window);
	SDL_GL_DeleteContext(glContext);
	SDL_Quit();

	delete shader;
	delete input;
	delete camera;
}

int main(int argc, char* argv[])
{
	init();
	setup();

	float lastFrameTime = SDL_GetTicks();

	while (!close)
	{
		float deltaTime = (SDL_GetTicks() - lastFrameTime) / 1000.0f;

		lastFrameTime = SDL_GetTicks();

		process_input();
		update(deltaTime);
		render();
	}

	cleanup();

	return 0;
}