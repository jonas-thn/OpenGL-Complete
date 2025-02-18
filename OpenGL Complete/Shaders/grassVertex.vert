#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexcoord;
layout (location = 2) in mat4 instanceModel;

out vec2 texcoord;

layout (std140) uniform Matrices
{
	mat4 proj;
	mat4 view;
};

uniform mat4 models[256];

void main()
{
	gl_Position = proj * view * models[gl_InstanceID] * vec4(aPos, 1.0);
	texcoord = aTexcoord;
}