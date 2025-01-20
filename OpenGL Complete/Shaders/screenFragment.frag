#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

const float offset = 1.0 / 300.0;

void main()
{
	FragColor = texture(screenTexture, TexCoords);
	
	//FragColor = vec4(1- texture(screenTexture, TexCoords).rgb, 1.0); //inverse

	//float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0;
	//FragColor = vec4(average, average, average, 1.0);

	vec2 offsets[9] = vec2[] 
	(
		vec2(-offset, offset),
		vec2(0.0f, offset),
		vec2(offset, offset),
		vec2(-offset, 0.0f),
		vec2(0.0f, 0.0f),
		vec2(offset, 0.0f),
		vec2(-offset, -offset),
		vec2(0.0f, -offset),
		vec2(offset, -offset)
	);

	float sharpenKernel[9] = float[] 
	(
		-1, -1, -1,
		-1, 9, -1,
		-1, -1, -1
	);

	float blurKernel[9] = float[]
	(
		1.0 / 16, 2.0 / 16, 1.0 / 16,
		2.0 / 16, 4.0 / 16, 2.0 / 16, 
		1.0 / 16, 2.0 / 16, 1.0 / 16
	);

	float edgeDetectionKernel[9] = float[]
	(
		1, 1, 1,
		1, -8, 1,
		1, 1, 1
	);

	vec3 sampleTex[9];
	for(int i = 0; i < 9; i ++)
	{
		sampleTex[i] = vec3(texture(screenTexture, TexCoords.rg + offsets[i]));
	}

	vec3 col = vec3(0.0);
	for(int i = 0; i < 9; i++)
	{
		col += sampleTex[i] * edgeDetectionKernel[i];
	}

	//FragColor = vec4(col, 1.0);
}