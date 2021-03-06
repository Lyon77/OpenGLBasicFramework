#shader vertex
#version 330 core
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;

out vec3 v_Normal;
out vec3 v_Position;

uniform mat4 u_MVP;
uniform mat4 u_Model;

void main()
{
	v_Normal = mat3(transpose(inverse(u_Model))) * normal;
	v_Position = vec3(u_Model * vec4(position, 1.0));
	gl_Position = u_MVP * vec4(position, 1.0);
};

#shader fragment
#version 330 core

out vec4 color;

in vec3 v_Normal;
in vec3 v_Position;

uniform vec3 u_CameraPos;
uniform samplerCube u_SkyBox;
uniform float u_Ratio;

void main()
{
	vec3 I = normalize(v_Position - u_CameraPos);
	vec3 R = refract(I, normalize(v_Normal), u_Ratio);
	color = vec4(texture(u_SkyBox, R).rgb, 1.0);
}