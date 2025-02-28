#pragma once

#define GLEW_STATIC
#include <GL/glew.h>

#include "stb_image.h"

#include "Shader.h"

class Material
{
public:
	static const int textureIndices[16];

	Material(int diffuseIndex, int specularIndex, float shininess, const char* diffusePath, const char* specularPath, const char* normalPath = NULL, int normalIndex = 0);

	~Material();

	void UseMaterial(Shader& shader);

private:
	int diffuseIndex;
	int specularIndex;
	int normalIndex = 0;
	float shininess;

	unsigned int diffuseTexture;
	unsigned int specularTexture;
	unsigned int normalTexture;
};


