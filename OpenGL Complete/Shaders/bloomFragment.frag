#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

int radius = 30;

void main()
{
	int counter = 0;

	vec2 tex_offset = 1.0 / textureSize(screenTexture, 0); 
	vec3 original = texture(screenTexture, TexCoords).rgb;
	vec3 result = texture(screenTexture, TexCoords).rgb; 
	for(int y = -radius; y < radius; ++y)
	{
		for(int x = -radius; x < radius; ++x)
		{
			if(vec2(x,y).length() > radius) continue;
			result += pow(texture(screenTexture, TexCoords + vec2(tex_offset.x * x, tex_offset.y * y)).rgb, vec3(4));
			counter++;
		}
		
	}
	counter = counter > 0 ? counter : 1;
	result /= counter;

	FragColor = vec4(original + result * vec3(1.0, 0.5, 0.7), 1.0);
}