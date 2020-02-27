#shader vertex
#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 u_MVP;

void main()
{
	//multiply position with projection matrix to account for window size
	gl_Position = u_MVP * vec4(position, 1.0);
};

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;
layout(location = 1) out vec4 brightColor;

uniform vec4 u_Color = vec4(1.0);

void main()
{
	//load texture
	color = u_Color;

	float brightness = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
	if (brightness > 1.0)
		brightColor = vec4(color.rgb, 1.0);
	else
		brightColor = vec4(0.0, 0.0, 0.0, 1.0);
};