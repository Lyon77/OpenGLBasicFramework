#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 bitTangent;

out vec3 v_FragPos;
out vec2 v_TexCoords;
out mat3 v_TBN;

uniform mat4 u_MVP;
uniform mat4 u_Model;

void main()
{
	gl_Position = u_MVP * vec4(position, 1.0);
	v_FragPos = vec3(u_Model * vec4(position, 1.0));
	v_TexCoords = texCoords;

	vec3 T = normalize(vec3(u_Model * vec4(tangent, 0.0)));
	vec3 B = normalize(vec3(u_Model * vec4(bitTangent, 0.0)));
	vec3 N = normalize(vec3(u_Model * vec4(normal, 0.0)));
	v_TBN = mat3(T, B, N);
};

#shader fragment
#version 330 core

out vec4 color;

in vec3 v_FragPos;
in vec2 v_TexCoords;
in mat3 v_TBN;

uniform sampler2D u_Texture;
uniform sampler2D u_NormalMap;

uniform vec3 u_ViewPos;

uniform struct PointLight {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
} u_PointLight;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
	vec3 lightDir = normalize(light.position - fragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);

	//Ambient Calculation
	vec3 ambient = light.ambient;

	//Diffuse Calculation
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = (diff) * light.diffuse;

	//Specular Calculation
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
	vec3 specular = (spec) * light.specular;

	return ambient + diffuse + specular;
}

void main()
{
	// obtain normal from normal map in range [0,1]
	vec3 normal = texture(u_NormalMap, v_TexCoords).rgb;
	// transform normal vector to range [-1,1]
	normal = normalize(normal * 2.0 - 1.0);

	normal = normalize(v_TBN * normal);

	//load texture
	vec3 texColor = texture(u_Texture, v_TexCoords).rgb;

	vec3 viewPos = normalize(u_ViewPos - u_PointLight.position);
	color = vec4(CalcPointLight(u_PointLight, normal, v_FragPos, viewPos) * texColor, 1.0);
};