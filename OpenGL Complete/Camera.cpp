#include "Camera.h"

Camera::Camera(int width, int height, Input& input) : width(width), height(height), input(input)
{
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	proj = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 50.0f);
	//proj = glm::ortho(0.0f, (float)width, 0.0f, (float)height, 0.1f, 100.0f);
}

void Camera::UpdateView(float deltaTime)
{
	viewDirection.x = cos(glm::radians(input.GetYaw())) * cos(glm::radians(input.GetPitch()));
	viewDirection.y = sin(glm::radians(input.GetPitch()));
	viewDirection.z = sin(glm::radians(input.GetYaw())) * cos(glm::radians(input.GetPitch()));

	cameraFront = glm::normalize(viewDirection);

	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

	if (input.GetW())
	{
		cameraPos += cameraSpeed * cameraFront * deltaTime;
	}
	if (input.GetS())
	{
		cameraPos -= cameraSpeed * cameraFront * deltaTime;
	}
	if (input.GetD())
	{
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * deltaTime;
	}
	if (input.GetA())
	{
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * deltaTime;
	}
}

glm::mat4 Camera::GetView()
{
	return view;
}

glm::mat4 Camera::GetProj()
{
	return proj;
}

glm::vec3 Camera::GetPos()
{
	return cameraPos;
}
