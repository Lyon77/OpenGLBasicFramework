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

float near = 0.1;
float far = 50.0;

float LinearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0;
	return (2.0 * near * far) / (far + near - z * (far - near));
}

void main()
{
	float depth = LinearizeDepth(gl_FragCoord.z) / far;
	color = vec4(vec3(depth), 1.0);
};