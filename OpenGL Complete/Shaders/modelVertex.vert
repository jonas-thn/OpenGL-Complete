#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexcoord;

layout (std140) uniform Matrices
{
	mat4 proj;
	mat4 view;
};

out vec2 texcoord;
out vec3 normal;
out vec3 fragPos;

uniform mat4 model;

void main()
{
	gl_Position = proj * view * model * vec4(aPos, 1.0);
	texcoord = aTexcoord;
	normal = mat3(transpose(inverse(model))) * aNormal;
	fragPos = vec3(model * vec4(aPos, 1.0));
}