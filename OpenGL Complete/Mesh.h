#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

class Mesh
{
public:
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	void Draw(Shader& shader);

private:

	unsigned int VAO, VBO, EBO;

	void SetupMesh();
};

