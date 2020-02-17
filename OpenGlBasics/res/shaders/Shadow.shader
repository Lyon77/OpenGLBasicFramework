#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

out vec3 v_FragPos;
out vec3 v_Normal;
out vec2 v_TexCoords;
out vec4 v_LightSpace;

uniform mat4 u_MVP;
uniform mat4 u_Model;
uniform mat4 u_LightSpaceMatrix;

void main()
{
	gl_Position = u_MVP * vec4(position, 1.0);
	v_FragPos = vec3(u_Model * vec4(position, 1.0));
	v_Normal = mat3(transpose(inverse(u_Model))) * normal;
	v_TexCoords = texCoords;
	v_LightSpace = u_LightSpaceMatrix * vec4(v_FragPos, 1.0);
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoords;
in vec4 v_LightSpace;

uniform vec3 u_ObjectColor;

uniform vec3 u_ViewPos;

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
uniform sampler2D u_ShadowMap;
uniform samplerCube u_ShadowCubeMap;
uniform float u_FarPlane;

//Function Prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
float CalcDirShadow(vec4 lightSpace, vec3 lightDir);
float CalcPointSpotShadow(vec3 fragPos, vec3 lightPos);

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

	color = vec4(result * u_ObjectColor, 1.0) * 1.5;

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
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), pow(2, u_Material.shininess));
	vec3 specular = (spec * vec3(texture(u_Material.specular, v_TexCoords))) * light.specular;

	//Shadow Calculation
	float shadow = CalcDirShadow(v_LightSpace, lightDir);

	return ambient + (diffuse + specular) * (1.0 - shadow);
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

	//Shadow Calculation
	float shadow = CalcPointSpotShadow(v_FragPos, light.position);

	return (ambient + (diffuse + specular) * (1.0 - shadow)) * attenuation;
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

	//Shadow Calculation
	float shadow = CalcPointSpotShadow(v_FragPos, light.position);

	return (ambient + (diffuse + specular) * (1.0 - shadow) * 10) * attenuation;
}

float CalcDirShadow(vec4 lightSpace, vec3 lightDir)
{
	vec3 projCoords = lightSpace.xyz / lightSpace.w;
	projCoords = projCoords * 0.5 + 0.5; //Change the range from [-1, 1] -> [0, 1]

	float closestDepth = texture(u_ShadowMap, projCoords.xy).r; // Get Closest Depth

	float currentDepth = projCoords.z; //Get Current Depth

	float bias = max(0.05 * (1.0 - dot(v_Normal, lightDir)), 0.005); //To eliminate artifacts from the depth buffer
	
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(u_ShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	shadow /= 9.0;

	if (projCoords.z > 1.0) //To deal with coordinates outside the far plane of	the light's orthographic frustum
		shadow = 0.0;

	return shadow;
}

float CalcPointSpotShadow(vec3 fragPos, vec3 lightPos)
{
	vec3 fragToLight = fragPos - lightPos;
	float currentDepth = length(fragToLight);

	vec3 sampleOffsetDirections[20] = vec3[]
	(
		vec3(1, 1, 1), vec3(1, -1, 1), vec3(-1, -1, 1), vec3(-1, 1, 1),
		vec3(1, 1, -1), vec3(1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
		vec3(1, 1, 0), vec3(1, -1, 0), vec3(-1, -1, 0), vec3(-1, 1, 0),
		vec3(1, 0, 1), vec3(-1, 0, 1), vec3(1, 0, -1), vec3(-1, 0, -1),
		vec3(0, 1, 1), vec3(0, -1, 1), vec3(0, -1, -1), vec3(0, 1, -1)
	);

	float shadow = 0.0;
	float bias = 0.15;
	float samples = 20;
	float diskRadius = (0.5 + (currentDepth / u_FarPlane)) / 25.0;
	for (int i = 0; i < samples; ++i)
	{
		float closestDepth = texture(u_ShadowCubeMap, fragToLight + sampleOffsetDirections[i] * diskRadius).r;
		closestDepth *= u_FarPlane;

		if(currentDepth - bias > closestDepth)
			shadow += 1.0;
	}


	shadow /= float(samples);

	return shadow;
}