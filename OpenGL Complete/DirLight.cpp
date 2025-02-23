#include "DirLight.h"

DirLight::DirLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
	:direction(direction), ambient(ambient), diffuse(diffuse), specular(specular) {
}

void DirLight::UseLight(Shader& shader)
{
	shader.SetVec3("dirLight.direction", direction);
	shader.SetVec3("dirLight.ambient", ambient);
	shader.SetVec3("dirLight.diffuse", diffuse);
	shader.SetVec3("dirLight.specular", specular);
}

glm::vec3 DirLight::GetDirection()
{
	return direction;
}
