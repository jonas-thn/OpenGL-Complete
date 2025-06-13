#version 330 core

layout(location = 0) out vec4 gPosition;
layout(location = 1) out vec4 gNormal;
layout(location = 2) out vec4 gAlbedo;

//out vec4 FragColor;

in VS_OUT
{
in vec2 texcoord;
in vec3 normal;
in vec3 fragPosWorld;
in vec3 fragPosView;
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
	vec3 viewDir = normalize(viewPos - fs_in.fragPosView);

	float depth = LinearDepth(gl_FragCoord.z, 0.1, 50);

	vec3 result = texture2D(material.diffuse, fs_in.texcoord).rgb;

	gPosition = vec4(fs_in.fragPosWorld, 1.0);
	gNormal = vec4(normalize(norm), 1.0);
	gAlbedo = vec4(texture2D(material.diffuse, fs_in.texcoord).rgb, 1.0);

	//	FragColor = vec4(result * 0.5, 1.0);

	//FragColor = vec4(vec3(depth), 1.0);
}