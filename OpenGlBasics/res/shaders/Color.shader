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

uniform vec3 u_ObjectColor;

uniform vec3 u_LightPos;
uniform vec3 u_ViewPos;

struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Light u_LightColor;

struct Material {
	sampler2D diffuse; // Ambient and Diffuse will be the same
	sampler2D specular;
	float shininess;
};

uniform Material u_Material;

void main()
{
	//Ambient Calculation
	vec3 ambient = vec3(texture(u_Material.diffuse, v_TexCoords)) * u_LightColor.ambient;
	
	//Diffuse Calculation
	vec3 norm = normalize(v_Normal);
	vec3 lightDir = normalize(u_LightPos - v_FragPos);

	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = (diff * vec3(texture(u_Material.diffuse, v_TexCoords))) * u_LightColor.diffuse;

	//Specular Calculation
	vec3 viewDir = normalize(u_ViewPos - v_FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);

	float spec = pow(max(dot(viewDir, reflectDir), 0.0), pow(2, u_Material.shininess));
	vec3 specular = (spec * vec3(texture(u_Material.specular, v_TexCoords))) * u_LightColor.specular;
	
	//Final Result
	vec3 result = (ambient + diffuse + specular) * u_ObjectColor;
	color = vec4(result, 1.0);
};