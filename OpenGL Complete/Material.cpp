#include "Material.h"

const int Material::textureIndices[16] =
{
	GL_TEXTURE0,
	GL_TEXTURE1,
	GL_TEXTURE2,
	GL_TEXTURE3,
	GL_TEXTURE4,
	GL_TEXTURE5,
	GL_TEXTURE6,
	GL_TEXTURE7,
	GL_TEXTURE8,
	GL_TEXTURE9,
	GL_TEXTURE10,
	GL_TEXTURE11,
	GL_TEXTURE12,
	GL_TEXTURE13,
	GL_TEXTURE14,
	GL_TEXTURE15,
};

Material::Material(int diffuseIndex, int specularIndex, float shininess, const char* diffusePath, const char* specularPath)
	:diffuseIndex(diffuseIndex), specularIndex(specularIndex), shininess(shininess)
{
	stbi_set_flip_vertically_on_load(true);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

	float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int width, height, nChannels;
	unsigned char* data = stbi_load(diffusePath, &width, &height, &nChannels, 0);

	if (data == NULL)
	{
		std::cout << "Failed to load texture." << std::endl;
	}

	glGenTextures(1, &diffuseTexture);
	glActiveTexture(textureIndices[diffuseIndex]);
	glBindTexture(GL_TEXTURE_2D, diffuseTexture);

	if (nChannels == 4)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	else if (nChannels == 3)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	else
	{
		std::cout << "nChannels Image Error." << std::endl;
	}
	glGenerateMipmap(GL_TEXTURE_2D);

	data = NULL;
	data = stbi_load(specularPath, &width, &height, &nChannels, 0);

	if (data == NULL)
	{
		std::cout << "Failed to load texture." << std::endl;
	}

	glGenTextures(1, &specularTexture);
	glActiveTexture(textureIndices[specularIndex]);
	glBindTexture(GL_TEXTURE_2D, specularTexture);

	if (nChannels == 4)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}
	else if (nChannels == 3)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	}
	else if (nChannels == 1)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_LUMINANCE, GL_UNSIGNED_BYTE, data);
	}
	else
	{
		std::cout << "nChannels Image Error." << std::endl;
	}

	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
}

Material::~Material()
{
	glDeleteTextures(1, &diffuseTexture);
	glDeleteTextures(1, &specularTexture);
}

void Material::UseMaterial(Shader& shader)
{
	glActiveTexture(textureIndices[diffuseIndex]);
	glBindTexture(GL_TEXTURE_2D, diffuseTexture);
	glActiveTexture(textureIndices[specularIndex]);
	glBindTexture(GL_TEXTURE_2D, specularTexture);

	shader.SetInt("material.diffuse", diffuseIndex);
	shader.SetInt("material.specular", specularIndex);
	shader.SetFloat("material.shininess", shininess);
}
