#pragma once

#include <vector>
#include <string>

#include "Shader.h"
#include "Mesh.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"

class Model
{
public:
	Model(const char* path)
	{
		LoadModel(path);
	}

	void Draw(Shader& shader);

private:

	std::vector<Mesh> meshes;
	std::string directory;

	void LoadModel(std::string path);
	void ProcessNode(aiNode *node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
};

