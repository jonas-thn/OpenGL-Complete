#include <iostream>
#include <memory>
#include <vector>
#include <map>

#define GLEW_STATIC
#include <GL/glew.h>

#define SDL_MAIN_HANDLED
#include <SDL.h>

#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/importer.hpp>

#include "Shader.h"
#include "Input.h"
#include "Camera.h"
#include "Material.h"
#include "PointLight.h"
#include "DirLight.h"
#include "Model.h"
#include "MeshData.h"

const int WIDTH = 800;
const int HEIGHT = 600;

SDL_Window* window;
SDL_GLContext glContext;
bool close = false;

unsigned int VBO, VAO;
unsigned int lightVBO, lightVAO;
unsigned int quadVBO, quadVAO;
unsigned int screenVBO, screenVAO;

unsigned int fbo;

Input* input = nullptr;
Camera* camera = nullptr;

Shader* shader = nullptr;
Shader* lightShader = nullptr;
Shader* modelShader = nullptr;
Shader* outlineShader = nullptr;
Shader* quadShader = nullptr;
Shader* screenShader = nullptr;

Material* material = nullptr;
Material* modelMaterial = nullptr;
Material* groundMaterial = nullptr;
Material* grassMaterial = nullptr;
Material* windowMaterial = nullptr;

Model* backpack = nullptr;

glm::vec3 lightPos1(0.0f, 0.0f, -3.0f);
const int NR_POINT_LIGHTS = 1;
std::vector<PointLight*> pointLights;
DirLight* dirLight = nullptr;

unsigned int textureColorbuffer;

int GetNextTextureIndex()
{
	static int number = 1;
	if(number < 30)
	{
		return number++;
	}
	else
	{
		return 0;
		std::cout << "Texture Index Error" << std::endl;
	}
}

void Init()
{
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

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
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);

	glEnable(GL_STENCIL_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	/*glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);*/

	SDL_ShowCursor(SDL_DISABLE);
	SDL_SetRelativeMouseMode(SDL_TRUE);
}

void Setup()
{
	//----------VBO, VAO, EBO----------

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	glGenVertexArrays(1, &lightVAO);
	glBindVertexArray(lightVAO);

		glGenBuffers(1, &lightVBO);
		glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(lightVertices), lightVertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	glGenVertexArrays(1, &quadVAO);
	glBindVertexArray(quadVAO);

		glGenBuffers(1, &quadVBO);
		glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	glGenVertexArrays(1, &screenVAO);
	glBindVertexArray(screenVAO);

		glGenBuffers(1, &screenVBO);
		glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(screenVertices), screenVertices, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	//texture framebuffer attachment
	glGenTextures(1, &textureColorbuffer);
	glActiveTexture(GL_TEXTURE31);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

	//renderbuffer object framebuffer attachment
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, WIDTH, HEIGHT);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Framebuffer Not Complete Error" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//----------INIT----------

	input = new Input();
	camera = new Camera(WIDTH, HEIGHT, *input);

	shader = new Shader("Shaders/vertex.vert", "Shaders/fragment.frag");
	lightShader = new Shader("Shaders/lightVertex.vert", "Shaders/lightFragment.frag");
	modelShader = new Shader("Shaders/modelVertex.vert", "Shaders/modelFragment.frag");
	outlineShader = new Shader("Shaders/outlineVertex.vert", "Shaders/outlineFragment.frag");
	quadShader = new Shader("Shaders/quadVertex.vert", "Shaders/quadFragment.frag");
	screenShader = new Shader("Shaders/screenVertex.vert", "Shaders/screenFragment.frag");

	PointLight* pointLight1 = new PointLight(lightPos1, glm::vec3(0.1, 0.1, 0.1), glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 1.0), 1.0f, 0.3, 0.2);
	pointLights.push_back(pointLight1);
	dirLight = new DirLight(glm::vec3(0.5, 1.0, 0.2), glm::vec3(0.1, 0.1, 0.1), glm::vec3(0.4, 0.4, 0.4), glm::vec3(1.0, 1.0, 1.0));

	material = new Material(GetNextTextureIndex(), GetNextTextureIndex(), 128, "./Textures/container2.png", "./Textures/container2_specular.png");
	modelMaterial = new Material(GetNextTextureIndex(), GetNextTextureIndex(), 128, "./Models/diffuse.jpg", "./Models/specular.jpg");
	grassMaterial = new Material(GetNextTextureIndex(), 0, 16, "./Textures/grass.png", "./Textures/no_specular.png");
	groundMaterial = new Material(GetNextTextureIndex(), 0, 16, "./Textures/concreteTexture.png", "./Textures/no_specular.png");
	windowMaterial = new Material(GetNextTextureIndex(), 0, 16, "./Textures/window.png", "./Textures/no_specular.png");

	backpack = new Model("./Models/backpack.obj");
}

void ProcessInput()
{
	input->HandleInput();
	close = input->GetQuit();
}
void Update(float deltaTime)
{
	camera->UpdateView(deltaTime);
}

void DrawScene()
{
	//----------LIGHT----------

	glStencilFunc(GL_ALWAYS, 1, 0xFF); //light komplett zu ref 1 setzen (ALWAYS = stencil test immer bestanden)
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); //nur setzen wenn depth und stencil test bestanden
	glStencilMask(0xFF); //stencil buffer schreiben an

	lightShader->UseShader();
	lightShader->SetVec3("lightColor", glm::vec3(1, 1, 1));
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos1);
	lightShader->SetMat4("model", lightModel);
	lightShader->SetMat4("view", camera->GetView());
	lightShader->SetMat4("proj", camera->GetProj());

	glBindVertexArray(lightVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	//----------OUTLINE----------

	glStencilFunc(GL_NOTEQUAL, 1, 0xFF); //wenn nicht 1, dann zeichnen
	glStencilMask(0x00); //stencil buffer schreiben deaktivieren

	outlineShader->UseShader();
	lightModel = glm::scale(lightModel, glm::vec3(1.1, 1.1, 1.1));
	outlineShader->SetMat4("model", lightModel);
	outlineShader->SetMat4("view", camera->GetView());
	outlineShader->SetMat4("proj", camera->GetProj());


	glBindVertexArray(lightVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	glStencilFunc(GL_ALWAYS, 1, 0xFF); //objekte mit ref 1 beschreiben

	//----------BOXES----------

	shader->UseShader();

	shader->SetVec3("viewPos", camera->GetPos());

	for (int i = 0; i < NR_POINT_LIGHTS; i++)
	{
		pointLights[i]->UseLight(*shader, i);
	}

	dirLight->UseLight(*shader);

	material->UseMaterial(*shader);

	shader->SetMat4("view", camera->GetView());
	shader->SetMat4("proj", camera->GetProj());

	glBindVertexArray(VAO);

	for (unsigned int i = 0; i < 10; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, cubePositions[i]);
		float angle = 20.0f * i;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		shader->SetMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	glBindVertexArray(0);

	//----------MODEL----------

	modelShader->UseShader();

	modelShader->SetVec3("viewPos", camera->GetPos());

	for (int i = 0; i < NR_POINT_LIGHTS; i++)
	{
		pointLights[i]->UseLight(*modelShader, i);
	}

	dirLight->UseLight(*modelShader);

	modelMaterial->UseMaterial(*modelShader);

	modelShader->SetMat4("view", camera->GetView());
	modelShader->SetMat4("proj", camera->GetProj());

	glm::mat4 backpackModel = glm::mat4(1.0f);
	backpackModel = glm::translate(backpackModel, glm::vec3(-5, 0, -5));
	modelShader->SetMat4("model", backpackModel);

	backpack->Draw(*modelShader);

	//----------GROUND----------

	quadShader->UseShader();
	glm::mat4 groundModel = glm::mat4(1.0f);
	groundModel = glm::scale(groundModel, glm::vec3(8.0f, 8.0f, 8.0f));
	groundModel = glm::rotate(groundModel, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	groundModel = glm::translate(groundModel, glm::vec3(0.9f, -0.4f, 0.3f));
	quadShader->SetMat4("model", groundModel);
	quadShader->SetMat4("view", camera->GetView());
	quadShader->SetMat4("proj", camera->GetProj());

	groundMaterial->UseMaterial(*quadShader);
	quadShader->SetVec3("colorTint", glm::vec3(0.5, 0.4, 0.3));

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	//----------GRASS----------

	quadShader->UseShader();
	quadShader->SetMat4("view", camera->GetView());
	quadShader->SetMat4("proj", camera->GetProj());

	grassMaterial->UseMaterial(*quadShader);
	quadShader->SetVec3("colorTint", glm::vec3(0.7, 0.7, 0.7));

	for (glm::vec3 pos : grassPositions)
	{
		glm::mat4 grassModel = glm::mat4(1.0f);
		grassModel = glm::translate(grassModel, pos);
		quadShader->SetMat4("model", grassModel);
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}

	//----------WINDOWS (transparent)----------

	//sort transparent objects by distance
	std::map<float, glm::vec3> sorted;

	for (unsigned int i = 0; i < windowPositions.size(); i++)
	{
		float distance = glm::length(camera->GetPos() - windowPositions[i]);
		sorted[distance] = windowPositions[i];
	}

	glm::mat4 windowModel;
	for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
	{
		windowModel = glm::mat4(1.0f);
		windowModel = glm::translate(windowModel, it->second);

		quadShader->UseShader();
		quadShader->SetMat4("view", camera->GetView());
		quadShader->SetMat4("proj", camera->GetProj());

		windowMaterial->UseMaterial(*quadShader);
		quadShader->SetVec3("colorTint", glm::vec3(1.0, 1.0, 1.0));

		quadShader->SetMat4("model", windowModel);

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}

	glStencilMask(0xFF);
}

void Render()
{
	//first pass
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClearColor(0.28f, 0.21f, 0.15f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	DrawScene();

	//second pass
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.28f, 0.21f, 0.15f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	screenShader->UseShader();
	glBindVertexArray(screenVAO);
	glDisable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE31);
	glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
	screenShader->SetInt("screenTexture", 31);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	SDL_GL_SwapWindow(window);
}

void Cleanup()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteVertexArrays(1, &quadVAO);

	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &lightVBO);
	glDeleteBuffers(1, &quadVBO);

	glDeleteFramebuffers(1, &fbo);

	delete shader;
	delete input;
	delete camera;
	delete lightShader;

	for (PointLight* light : pointLights)
	{
		delete light;
	}

	delete dirLight;
	delete material;
	delete backpack;
	delete modelShader;
	delete modelMaterial;
	delete outlineShader;
	delete quadShader;
	delete grassMaterial;
	delete groundMaterial;
	delete windowMaterial;

	SDL_DestroyWindow(window);
	SDL_GL_DeleteContext(glContext);
	SDL_Quit();
}

int main(int argc, char* argv[])
{
	Init();
	Setup();

	float lastFrameTime = SDL_GetTicks();

	while (!close)
	{
		float deltaTime = (SDL_GetTicks() - lastFrameTime) / 1000.0f;

		lastFrameTime = SDL_GetTicks();

		ProcessInput();
		Update(deltaTime);
		Render();
	}

	Cleanup();

	return 0;
}