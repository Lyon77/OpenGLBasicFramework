#shader vertex
#version 330 core
layout(location = 0) in vec3 position;

out vec3 v_TexCoords;

uniform mat4 u_MVP;

void main()
{
	vec4 pos = u_MVP * vec4(position, 1.0);
	gl_Position = pos.xyww;
	v_TexCoords = position;
};

#shader fragment
#version 330 core

out vec4 color;

in vec3 v_TexCoords;

uniform samplerCube u_SkyBox;

void main()
{
	color = texture(u_SkyBox, v_TexCoords);
}