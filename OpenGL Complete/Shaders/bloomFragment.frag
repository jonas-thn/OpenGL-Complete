#version 330 core

out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gAlbedo;
uniform sampler2D gPosition;
uniform sampler2D gNormal;

uniform mat4 projection;
uniform mat4 view;

uniform vec3 backgroundColor;

int bloomRadius = 30;

float ssaoRadius = 0.5;
const int kernelSize = 16;
vec3 samples[kernelSize] = vec3[](
    vec3( 0.5381,  0.1856,  0.4319),
    vec3( 0.1379,  0.2486,  0.4430),
    vec3( 0.3371,  0.5679,  0.0057),
    vec3(-0.6999, -0.0451,  0.0019),
    vec3( 0.0689, -0.1598, -0.8547),
    vec3( 0.0560,  0.0069, -0.1843),
    vec3(-0.0146,  0.1402,  0.0762),
    vec3( 0.0100, -0.1924, -0.0344),
    vec3(-0.3577, -0.5301, -0.4358),
    vec3(-0.3169,  0.1063,  0.0158),
    vec3( 0.0103, -0.5869,  0.0046),
    vec3(-0.0897, -0.4940,  0.3287),
    vec3( 0.7119, -0.0154, -0.0918),
    vec3(-0.0533,  0.0596, -0.5411),
    vec3( 0.0352, -0.0631,  0.5460),
    vec3(-0.4776,  0.2847, -0.0271)
);

bool similarVec3(vec3 a, vec3 b, float epsilon)
{
    return all(lessThanEqual(abs(a - b), vec3(epsilon)));
}

void main()
{
    //bloom
	int counter = 0;

	vec2 tex_offset = 1.0 / textureSize(gAlbedo, 0); 
	vec3 original = texture(gAlbedo, TexCoords).rgb;
	vec3 result = texture(gAlbedo, TexCoords).rgb; 
	for(int y = -bloomRadius; y < bloomRadius; ++y)
	{
		for(int x = -bloomRadius; x < bloomRadius; ++x)
		{
			if(vec2(x,y).length() > bloomRadius) continue;
			result += pow(texture(gAlbedo, TexCoords + vec2(tex_offset.x * x, tex_offset.y * y)).rgb, vec3(8));
			counter++;
		}
		
	}
	counter = counter > 0 ? counter : 1;
	result /= counter;

	//ssao
	vec3 pos = texture(gPosition, TexCoords).xyz;
    vec3 normal = normalize(texture(gNormal, TexCoords).xyz);
	normal = normalize(mat3(view) * normal);

    float occlusion = 0.0;
    for (int i = 0; i < kernelSize; i++)
    {
        vec3 viewPos = (view * vec4(pos, 1.0)).xyz;
		vec3 samplePos = viewPos + samples[i] * ssaoRadius;
		vec4 offset = projection * vec4(samplePos, 1.0);
		offset.xyz /= offset.w;
		vec2 sampleTexCoords = offset.xy * 0.5 + 0.5;

        float sampleDepth = texture(gPosition, sampleTexCoords).z;
        float rangeCheck = smoothstep(0.0, 1.0, ssaoRadius / abs(pos.z - sampleDepth));
        if (sampleDepth >= samplePos.z)
            occlusion += rangeCheck;
    }
	float ao = (occlusion / kernelSize);
    vec3 maskColor = pow(original, vec3(8));
    float mask = 1 - (original.r / 3 + original.g / 3 + original.b / 3);
    vec3 additionalMask = vec3(1.0 - result.r, 1.0 - result.g, 1.0 - result.b);
    ao = mix(1.0, ao, (additionalMask.r / 3 + additionalMask.g / 3 + additionalMask.b / 3) * mask);

    ao = pow(ao, 5.0);

    if(similarVec3(original, backgroundColor, 0.01))
    {
        discard;
    }

	FragColor = vec4(original  * ao + result * vec3(1.0, 0.5, 0.7), 1.0);

}