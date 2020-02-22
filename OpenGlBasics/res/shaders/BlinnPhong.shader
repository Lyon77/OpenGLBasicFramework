#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

out vec3 v_FragPos;
out vec3 v_Normal;
out vec2 v_TexCoords;

uniform mat4 u_MVP;
uniform mat4 u_Model;

void main()
{
	gl_Position = u_MVP * vec4(position, 1.0);
	v_FragPos = vec3(u_Model * vec4(position, 1.0));
	v_Normal = mat3(transpose(inverse(u_Model))) * normal;
	v_TexCoords = texCoords;
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoords;

uniform vec3 u_ViewPos;
uniform vec3 u_ObjectColor;

struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform bool u_HasDirLight = false;
uniform DirLight u_DirLight;

struct PointLight {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

uniform int u_NumPointLights = 0;
uniform PointLight u_PointLight[10]; //Current Max is 10

struct SpotLight {
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

uniform bool u_HasSpotLight = false;
uniform SpotLight u_SpotLight; //Current Max is 10

struct Material {
	sampler2D diffuse; // Ambient and Diffuse will be the same
	sampler2D specular;
	float shininess;
};

uniform Material u_Material;
uniform bool u_Gamma;
uniform bool u_Att;

//Function Prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
	// properties
	vec3 norm = normalize(v_Normal);
	vec3 viewDir = normalize(u_ViewPos - v_FragPos);

	vec3 result = vec3(0.0);

	//Calculate Dir Light
	if (u_HasDirLight)
	{
		result += CalcDirLight(u_DirLight, norm, viewDir);
	}

	//Calculate PointLights
	for (int i = 0; i < u_NumPointLights; i++)
	{
		result += CalcPointLight(u_PointLight[i], norm, v_FragPos, viewDir);
	}

	//Calculate SpotLights
	if (u_HasSpotLight)
	{
		result += CalcSpotLight(u_SpotLight, norm, v_FragPos, viewDir);
	}

	color = vec4(result * u_ObjectColor, 1.0);
	
	if (u_Gamma)
	{
		//Add Gamma Correction
		color.rgb = pow(color.rgb, vec3(1.0 / 2.2));
	}
};

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-light.direction);
	vec3 halfwayDir = normalize(lightDir + viewDir);

	//Ambient Calculation
	vec3 ambient = vec3(texture(u_Material.diffuse, v_TexCoords)) * light.ambient;

	//Diffuse Calculation
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = (diff * vec3(texture(u_Material.diffuse, v_TexCoords))) * light.diffuse;

	//Specular Calculation
	float spec = pow(max(dot(normal, halfwayDir), 0.0), pow(2, u_Material.shininess));
	vec3 specular = (spec * vec3(texture(u_Material.specular, v_TexCoords))) * light.specular;

	return ambient + diffuse + specular;
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);

	//Ambient Calculation
	vec3 ambient = vec3(texture(u_Material.diffuse, v_TexCoords)) * light.ambient;

	//Diffuse Calculation
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = (diff * vec3(texture(u_Material.diffuse, v_TexCoords))) * light.diffuse;

	//Specular Calculation
	float spec = pow(max(dot(normal, halfwayDir), 0.0), pow(2, u_Material.shininess));
	vec3 specular = (spec * vec3(texture(u_Material.specular, v_TexCoords))) * light.specular;

	// simple attenuation
	float distance = length(light.position - v_FragPos);
	float attenuation = 1.0 / (u_Gamma ? distance * distance : distance);

	if (!u_Att)
	{
		attenuation = 1.0;
	}

	return (ambient + diffuse + specular) * attenuation;
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);

	//Ambient Calculation
	vec3 ambient = vec3(texture(u_Material.diffuse, v_TexCoords)) * light.ambient;

	//Diffuse Calculation
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = (diff * vec3(texture(u_Material.diffuse, v_TexCoords))) * light.diffuse;

	//Specular Calculation
	float spec = pow(max(dot(normal, halfwayDir), 0.0), pow(2, u_Material.shininess));
	vec3 specular = (spec * vec3(texture(u_Material.specular, v_TexCoords))) * light.specular;

	//SoftEdges SpotLight
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = (light.cutOff - light.outerCutOff);
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	diffuse *= intensity;
	specular *= intensity;

	// simple attenuation
	float distance = length(light.position - v_FragPos);
	float attenuation = 1.0 / (u_Gamma ? distance * distance : distance);

	if (!u_Att)
	{
		attenuation = 1.0;
	}

	return (ambient + diffuse + specular) * attenuation;
}