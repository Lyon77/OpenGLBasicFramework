#shader vertex
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aNormal;
layout(location = 2) in vec2 aTexCoords;

out vec2 v_TexCoord;

uniform mat4 u_MVP;

void main()
{
	//multiply position with projection matrix to account for window size
	gl_Position = u_MVP * vec4(aPos, 1.0);
	v_TexCoord = aTexCoords;
};

#shader fragment
#version 330 core

out vec4 color;

in vec2 v_TexCoord;

uniform sampler2D texture_diffuse1;

void main()
{
	//load texture
	color = texture(texture_diffuse1, v_TexCoord);
};