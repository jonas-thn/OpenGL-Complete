#version 330 core

out vec4 FragColor;

in VS_OUT
{
in vec2 texcoord;
in vec3 normal;
in vec3 fragPos;
} fs_in;

uniform vec3 viewPos;

struct Material 
{
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

uniform Material material;

float LinearDepth(float depth, float near, float far)
{
	return near / (far - depth * (far - near));
}

void main()
{
	vec3 norm = normalize(fs_in.normal);
	vec3 viewDir = normalize(viewPos - fs_in.fragPos);

	float depth = LinearDepth(gl_FragCoord.z, 0.1, 50);

	vec3 result = texture2D(material.diffuse, fs_in.texcoord).rgb;

	FragColor = vec4(result * 0.5, 1.0);

	//FragColor = vec4(vec3(depth), 1.0);
}