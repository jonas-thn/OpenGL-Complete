#include <iostream>
#include <memory>
#include <vector>
#include <map>
#include <cstdlib>
#include <ctime>

#define GLEW_STATIC
#include <GL/glew.h>

#define SDL_MAIN_HANDLED
#include <SDL.h>

#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/importer.hpp>

#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_sdl2.h"
#include "ImGui/imgui_impl_opengl3.h"

#include "Shader.h"
#include "Input.h"
#include "Camera.h"
#include "Material.h"
#include "PointLight.h"
#include "DirLight.h"
#include "Model.h"
#include "MeshData.h"
#include "Skybox.h"

const int WIDTH = 800;
const int HEIGHT = 600;

SDL_Window* window;
SDL_GLContext glContext;
bool close = false;

unsigned int VBO, VAO;
unsigned int lightVBO, lightVAO;
unsigned int quadVBO, quadVAO;
unsigned int screenVBO, screenVAO;
unsigned int cubeTextureVBO, cubeTextureVAO;
unsigned int cubeNormalVBO, cubeNormalVAO;
unsigned int houseVBO, houseVAO;

unsigned int fbo;

Input* input = nullptr;
Camera* camera = nullptr;

Shader* shader = nullptr;
Shader* lightShader = nullptr;
Shader* modelShader = nullptr;
Shader* outlineShader = nullptr;
Shader* quadShader = nullptr;
Shader* screenShader = nullptr;
Shader* skyboxShader = nullptr;
Shader* reflectionShader = nullptr;
Shader* houseShader = nullptr;
Shader* grassShader = nullptr;

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

Skybox* skybox = nullptr;

unsigned int uboMatrices;

unsigned int const grassAmount = 256;
glm::mat4 grassModels[grassAmount];

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

float GenerateRandomFloat(float min, float max) 
{
	return min + static_cast<float>(rand()) / RAND_MAX * (max - min);
}

void Init()
{
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

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

	glEnable(GL_MULTISAMPLE);
	glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);

	//glEnable(GL_FRAMEBUFFER_SRGB);

	SDL_ShowCursor(SDL_DISABLE);
	SDL_SetRelativeMouseMode(SDL_TRUE);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(window, glContext);
	ImGui_ImplOpenGL3_Init("#version 130");
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

	glGenVertexArrays(1, &cubeTextureVAO);
	glGenBuffers(1, &cubeTextureVBO);
	glBindVertexArray(cubeTextureVAO);
		glBindBuffer(GL_ARRAY_BUFFER, cubeTextureVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeTextureVertices), &cubeTextureVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);

	glGenVertexArrays(1, &cubeNormalVAO);
	glGenBuffers(1, &cubeNormalVBO);
	glBindVertexArray(cubeNormalVAO);
		glBindBuffer(GL_ARRAY_BUFFER, cubeNormalVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeNormalVertices), &cubeNormalVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);

	glGenVertexArrays(1, &houseVAO);
	glGenBuffers(1, &houseVBO);
	glBindVertexArray(houseVAO);
		glBindBuffer(GL_ARRAY_BUFFER, houseVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(points), &points, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
	glBindVertexArray(0);

	//----------INIT----------

	input = new Input();
	camera = new Camera(WIDTH, HEIGHT, *input);

	shader = new Shader("Shaders/vertex.vert", "Shaders/fragment.frag");
	lightShader = new Shader("Shaders/lightVertex.vert", "Shaders/lightFragment.frag");
	modelShader = new Shader("Shaders/modelVertex.vert", "Shaders/modelFragment.frag");
	outlineShader = new Shader("Shaders/outlineVertex.vert", "Shaders/outlineFragment.frag");
	quadShader = new Shader("Shaders/quadVertex.vert", "Shaders/quadFragment.frag");
	screenShader = new Shader("Shaders/screenVertex.vert", "Shaders/screenFragment.frag");
	skyboxShader = new Shader("Shaders/skyboxVertex.vert", "Shaders/skyboxFragment.frag");
	reflectionShader = new Shader("Shaders/reflectionVertex.vert", "Shaders/reflectionFragment.frag");
	houseShader = new Shader("Shaders/houseVertex.vert", "Shaders/houseFragment.frag", "Shaders/houseGeometry.geom");
	grassShader = new Shader("Shaders/grassVertex.vert", "Shaders/grassFragment.frag");

	PointLight* pointLight1 = new PointLight(lightPos1, glm::vec3(0.1, 0.1, 0.1), glm::vec3(1.0, 1.0, 1.0), glm::vec3(1.0, 1.0, 1.0), 1.0f, 0.3, 0.2);
	pointLights.push_back(pointLight1);
	dirLight = new DirLight(glm::vec3(0.5, 1.0, 0.2), glm::vec3(0.1, 0.1, 0.1), glm::vec3(0.4, 0.4, 0.4), glm::vec3(1.0, 1.0, 1.0));

	material = new Material(GetNextTextureIndex(), GetNextTextureIndex(), 128, "./Textures/container2.png", "./Textures/container2_specular.png");
	modelMaterial = new Material(GetNextTextureIndex(), GetNextTextureIndex(), 128, "./Models/diffuse.jpg", "./Models/specular.jpg");
	grassMaterial = new Material(GetNextTextureIndex(), 0, 16, "./Textures/grass.png", "./Textures/no_specular.png");
	groundMaterial = new Material(GetNextTextureIndex(), 0, 16, "./Textures/concreteTexture.png", "./Textures/no_specular.png");
	windowMaterial = new Material(GetNextTextureIndex(), 0, 16, "./Textures/window.png", "./Textures/no_specular.png");

	backpack = new Model("./Models/backpack.obj");

	skybox = new Skybox();

	unsigned int matricesIndex = glGetUniformBlockIndex(shader->GetID(), "Matrices");
	unsigned int reflectionMatricesIndex = glGetUniformBlockIndex(reflectionShader->GetID(), "Matrices");
	unsigned int quadMatricesIndex = glGetUniformBlockIndex(quadShader->GetID(), "Matrices");
	unsigned int outlineMatricesIndex = glGetUniformBlockIndex(outlineShader->GetID(), "Matrices");
	unsigned int modelMatricesIndex = glGetUniformBlockIndex(modelShader->GetID(), "Matrices");
	unsigned int lightMatricesIndex = glGetUniformBlockIndex(lightShader->GetID(), "Matrices");
	
	glUniformBlockBinding(shader->GetID(), matricesIndex, 0);
	glUniformBlockBinding(reflectionShader->GetID(), reflectionMatricesIndex, 0);
	glUniformBlockBinding(quadShader->GetID(), quadMatricesIndex, 0);
	glUniformBlockBinding(outlineShader->GetID(), outlineMatricesIndex, 0);
	glUniformBlockBinding(modelShader->GetID(), modelMatricesIndex, 0);
	glUniformBlockBinding(lightShader->GetID(), lightMatricesIndex, 0);

	glGenBuffers(1, &uboMatrices);

	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, 2 * sizeof(glm::mat4));

	glm::mat4 projection = camera->GetProj();
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(projection));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	for (int i = 0; i < grassAmount; i++)
	{
		glm::mat4 tempModel = glm::mat4(1.0f);	
		glm::mat4 tempRot = glm::rotate(glm::mat4(1.0f), glm::radians(GenerateRandomFloat(0, 360)), glm::vec3(0, 1, 0));
		float xGrass = GenerateRandomFloat(3.5f, 11.0f);
		float yGrass = GenerateRandomFloat(-7.0f, 0.5f);
		//std::cout << "x: " << xGrass << " y: " << yGrass << std::endl;
		glm::mat4 tempTrans = glm::translate(glm::mat4(1.0f), glm::vec3(xGrass, -1.9f, yGrass));
		tempModel = tempTrans * tempRot;
		grassModels[i] = tempModel;
	}
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

void RenderImGui(bool x)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();

	if(x)
	{
		ImGui::SetNextWindowPos(ImVec2(0, 0));

		ImGui::Begin("Scenes", NULL, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

		ImGui::Button("Scene 1", ImVec2(100, 50));
		ImGui::Button("Scene 2", ImVec2(100, 50));
		ImGui::Button("Scene 3", ImVec2(100, 50));

		ImGui::End();
	}

	ImGui::Render();
	//glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	/*glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);*/
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

	glBindVertexArray(lightVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	//----------OUTLINE----------

	glStencilFunc(GL_NOTEQUAL, 1, 0xFF); //wenn nicht 1, dann zeichnen
	glStencilMask(0x00); //stencil buffer schreiben deaktivieren

	outlineShader->UseShader();
	lightModel = glm::scale(lightModel, glm::vec3(1.1, 1.1, 1.1));
	outlineShader->SetMat4("model", lightModel);

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

	groundMaterial->UseMaterial(*quadShader);
	quadShader->SetVec3("colorTint", glm::vec3(0.5, 0.4, 0.3));

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);

	//----------GRASS----------

	grassShader->UseShader();

	grassMaterial->UseMaterial(*grassShader);
	grassShader->SetVec3("colorTint", glm::vec3(0.7, 0.7, 0.7));

	for (int i = 0; i < grassAmount; i++)
	{
		grassShader->SetMat4("models[" + std::to_string(i) + "]", grassModels[i]);
	}
	glBindVertexArray(quadVAO);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 6, grassAmount);
	glBindVertexArray(0);

	//----------REFLECTION CUBE----------

	reflectionShader->UseShader();
	glm::mat4 reflectionModel = glm::mat4(1.0f);
	reflectionModel = glm::translate(reflectionModel, glm::vec3(5, 0, 0));

	reflectionShader->SetMat4("model", reflectionModel);

	reflectionShader->SetVec3("cameraPos", camera->GetPos());
	reflectionShader->SetInt("shouldRefract", 0);

	int skyboxTextureIndex = GetNextTextureIndex();
	glActiveTexture(GL_TEXTURE0 + skyboxTextureIndex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->GetCubemapTexture());
	reflectionShader->SetInt("cubemapTexture", skyboxTextureIndex);

	glBindVertexArray(cubeNormalVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(cubeNormalVAO);

	//----------REFRACTION CUBE----------

	reflectionShader->UseShader();
	glm::mat4 refractionModel = glm::mat4(1.0f);
	refractionModel = glm::translate(refractionModel, glm::vec3(4, 0, 0));

	reflectionShader->SetMat4("model", refractionModel);

	reflectionShader->SetVec3("cameraPos", camera->GetPos());
	reflectionShader->SetInt("shouldRefract", 1);

	glActiveTexture(GL_TEXTURE0 + skyboxTextureIndex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->GetCubemapTexture());
	reflectionShader->SetInt("cubemapTexture", skyboxTextureIndex);

	glBindVertexArray(cubeNormalVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(cubeNormalVAO);

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

		windowMaterial->UseMaterial(*quadShader);
		quadShader->SetVec3("colorTint", glm::vec3(1.0, 1.0, 1.0));

		quadShader->SetMat4("model", windowModel);

		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
	}

	//----------POINTS -> HOUSE----------
	/*houseShader->UseShader();
	glBindVertexArray(houseVAO);
	glDrawArrays(GL_POINTS, 0, 4);
	glBindVertexArray(0);*/

	glStencilMask(0xFF);
}

void Render()
{
	glm::mat4 view = camera->GetView();
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	//first pass
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClearColor(0.28f, 0.21f, 0.15f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	//skybox
	glDisable(GL_STENCIL_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);
	skybox->Draw(*skyboxShader, *camera, GetNextTextureIndex());
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	glEnable(GL_STENCIL_TEST);

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

	RenderImGui(input->GetX());

	SDL_GL_SwapWindow(window);
}

void Cleanup()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &lightVAO);
	glDeleteVertexArrays(1, &quadVAO);
	glDeleteVertexArrays(1, &screenVAO);
	glDeleteVertexArrays(1, &cubeTextureVAO);
	glDeleteVertexArrays(1, &cubeNormalVAO);
	glDeleteVertexArrays(1, &houseVAO);

	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &lightVBO);
	glDeleteBuffers(1, &quadVBO);
	glDeleteBuffers(1, &screenVBO);
	glDeleteBuffers(1, &cubeTextureVBO);
	glDeleteBuffers(1, &cubeNormalVBO);
	glDeleteBuffers(1, &houseVBO);

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
	delete skyboxShader;
	delete reflectionShader;
	delete houseShader;
	delete grassShader;

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