#shader vertex
#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoords;

out vec2 v_TexCoords;

uniform mat4 u_Projection;

void main()
{
	gl_Position = u_Projection * vec4(position, 0.0, 1.0);
	v_TexCoords = texCoords;
}

#shader fragment
#version 330 core

out vec4 color;

in vec2 v_TexCoords;

uniform sampler2D u_Text;
uniform vec3 u_TextColor;

void main()
{
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(u_Text, v_TexCoords).r);
	color = vec4(u_TextColor, 1.0) * sampled;
}