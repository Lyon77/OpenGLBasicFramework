#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

out vec2 v_TexCoords;
out vec3 v_FragPos;
out vec3 v_Normal;

uniform mat4 u_MVP;
uniform mat4 u_Model;

void main()
{
	vec4 worldPos = u_Model * vec4(position, 1.0);
	v_FragPos = worldPos.xyz;
	v_TexCoords = texCoords;

	mat3 normalMatrix = transpose(inverse(mat3(u_Model)));
	v_Normal = normalMatrix * normal;

	gl_Position = u_MVP * worldPos;
};

#shader fragment
#version 330 core

layout(location = 0) out vec3 position;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec4 albedoSpec;

in vec2 v_TexCoords;
in vec3 v_FragPos;
in vec3 v_Normal;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;

void main()
{
	// store the fragment position vector in the first gbuffer texture
	position = v_FragPos;
	// also store the per-fragment normals into the gbuffer
	normal = normalize(v_Normal);
	// and the diffuse per-fragment color
	albedoSpec.rgb = texture(texture_diffuse1, v_TexCoords).rgb;
	// store specular intensity in gAlbedoSpec's alpha component
	albedoSpec.a = texture(texture_specular1, v_TexCoords).r;
}