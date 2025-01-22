#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform samplerCube skybox;
uniform int shouldRefract;

void main()
{
	if (shouldRefract == 1)
	{
		float ratio = 1.0 / 1.2;
		vec3 I = normalize(Position - cameraPos);
		vec3 R = refract(I, normalize(Normal), ratio);
		vec3 color = vec3(texture(skybox, R).rgb) * 0.2 + vec3(0.6, 0.5, 0.4) * 0.8;
		FragColor = vec4(color, 0.7);
	}
	else
	{
		vec3 I = normalize(Position - cameraPos);
		vec3 R = reflect(I, normalize(Normal));
		FragColor = vec4(texture(skybox, R).rgb, 1.0);
	}
}