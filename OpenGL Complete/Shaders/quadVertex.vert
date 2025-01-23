#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexcoord;

out vec2 texcoord;

layout (std140) uniform Matrices
{
	mat4 proj;
	mat4 view;
};

uniform mat4 model;

void main()
{
	gl_Position = proj * view * model * vec4(aPos, 1.0);
	texcoord = aTexcoord;
}