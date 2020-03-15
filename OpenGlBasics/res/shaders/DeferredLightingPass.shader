#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoords;

out vec2 v_TexCoords;

void main()
{
	v_TexCoords = texCoords;
	gl_Position = vec4(position, 1.0);
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoords;

uniform sampler2D u_Position;
uniform sampler2D u_Normal;
uniform sampler2D u_AlbedoSpec;

struct Light
{
	vec3 Position;
	vec3 Color;
	float Linear;
	float Quadratic;
	float Radius;
};

uniform int u_NbrLights;
uniform Light u_Lights[100]; // Max 100 Lights
uniform vec3 u_ViewPos;

void main()
{
	// retrieve data from gbuffer
	vec3 FragPos = texture(u_Position, v_TexCoords).rgb;
	vec3 Normal = texture(u_Normal, v_TexCoords).rgb;
	vec3 Diffuse = texture(u_AlbedoSpec, v_TexCoords).rgb;
	float Specular = texture(u_AlbedoSpec, v_TexCoords).a;

	// then calculate lighting as usual
	vec3 lighting = Diffuse * 0.1; // hard-coded ambient component
	vec3 viewDir = normalize(u_ViewPos - FragPos);
	for (int i = 0; i < u_NbrLights; ++i)
	{
		// calculate distance between light source and current fragment
		float distance = length(u_Lights[i].Position - FragPos);
		if (distance < u_Lights[i].Radius)
		{
			// diffuse
			vec3 lightDir = normalize(u_Lights[i].Position - FragPos);
			vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * u_Lights[i].Color;
			// specular
			vec3 halfwayDir = normalize(lightDir + viewDir);
			float spec = pow(max(dot(Normal, halfwayDir), 0.0), 32.0);
			vec3 specular = u_Lights[i].Color * spec * Specular;
			// attenuation
			float attenuation = 1.0 / (1.0 + u_Lights[i].Linear * distance + u_Lights[i].Quadratic * distance * distance);
			diffuse *= attenuation;
			specular *= attenuation;
			lighting += diffuse + specular;
		}
	}

	color = vec4(lighting, 1.0);

}