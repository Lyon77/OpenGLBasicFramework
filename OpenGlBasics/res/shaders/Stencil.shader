#shader vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoords;

uniform mat4 u_MVP;
uniform mat4 u_Model;

void main()
{
	gl_Position = u_MVP * vec4(position, 1.0);
};

#shader fragment
#version 330 core

out vec4 color;

void main()
{
	color = vec4(0.04, 0.28, 0.26, 1.0);
};