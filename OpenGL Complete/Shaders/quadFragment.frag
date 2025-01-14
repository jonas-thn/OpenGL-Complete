#version 330 core

out vec4 FragColor;

in vec2 texcoord;

struct Material 
{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

uniform Material material;

uniform vec3 colorTint;

void main()
{
	vec4 tex = texture(material.diffuse, texcoord);
	FragColor = vec4(tex.rgb * colorTint, tex.a);
}