#include "PointLight.h"

PointLight::PointLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic)
	: position(position), ambient(ambient), diffuse(diffuse), specular(specular), constant(constant), linear(linear), quadratic(quadratic) {
}

void PointLight::UseLight(Shader& shader, int index)
{
	shader.SetVec3("pointLights[" + std::to_string(index) + "].position", position);
	shader.SetVec3("pointLights[" + std::to_string(index) + "].ambient", ambient);
	shader.SetVec3("pointLights[" + std::to_string(index) + "].diffuse", diffuse);
	shader.SetVec3("pointLights[" + std::to_string(index) + "].specular", specular);
	shader.SetFloat("pointLights[" + std::to_string(index) + "].constant", constant);
	shader.SetFloat("pointLights[" + std::to_string(index) + "].linear", linear);
	shader.SetFloat("pointLights[" + std::to_string(index) + "].quadratic", quadratic);
}


