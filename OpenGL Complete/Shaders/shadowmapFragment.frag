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

struct DirLight
{
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform DirLight dirLight;

//struct PointLight
//{
//	vec3 position;
//
//	float constant;
//	float linear;
//	float quadratic;
//
//	vec3 ambient;
//	vec3 diffuse;
//	vec3 specular;
//};
//
//#define NR_POINT_LIGHTS 1
//uniform PointLight pointLights[NR_POINT_LIGHTS];

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);

	//diffuse
	float diff = max(dot(normal, lightDir), 0.0);

	//specular
	vec3 reflectDir = reflect(-lightDir, normal);
	vec3 halfwayDir = normalize(lightDir + viewDir); 
	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

	//combine
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, fs_in.texcoord));
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, fs_in.texcoord));
	vec3 specular = light.specular * spec * vec3(texture(material.specular, fs_in.texcoord));

	return (ambient + diffuse + specular);
}

//vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
//{
//	vec3 lightDir = normalize(light.position - fragPos);
//
//	//diffuse 
//	float diff = max(dot(normal, lightDir), 0.0);
//
//	//specular
//	vec3 reflectDir = reflect(-lightDir, normal);
//	vec3 halfwayDir = normalize(lightDir + viewDir); 
//	float spec = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
//
//	//attenuation
//	float distance = length(light.position - fragPos);
//	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
//	
//	//combine
//	vec3 ambient = light.ambient * vec3(texture(material.diffuse, fs_in.texcoord));
//	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, fs_in.texcoord));
//	vec3 specular = light.specular * spec * vec3(texture(material.specular, fs_in.texcoord));
//
//	ambient *= attenuation;
//	diffuse *= attenuation;
//	specular *= attenuation;
//
//	return (ambient + diffuse + specular);
//}

void main()
{
	vec3 norm = normalize(fs_in.normal);
	vec3 viewDir = normalize(viewPos - fs_in.fragPos);

	//dir light
	vec3 result = CalcDirLight(dirLight, norm, viewDir);

	//point lights
//	for(int i = 0; i < NR_POINT_LIGHTS; i++)
//	{
//		result += CalcPointLight(pointLights[i], norm, fs_in.fragPos, viewDir);
//	}

	FragColor = vec4(result, 1.0);
	//FragColor = vec4(vec3(depth), 1.0);
}