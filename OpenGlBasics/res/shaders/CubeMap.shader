#shader vertex
#version 330 core
layout(location = 0) in vec3 position;

out vec3 v_TexCoords;

uniform mat4 u_VP;

void main()
{
	vec4 pos = u_VP * vec4(position, 1.0);
	gl_Position = pos.xyww;
	v_TexCoords = position;
};

#shader fragment
#version 330 core

out vec4 color;

in vec3 v_TexCoords;

uniform samplerCube u_SkyBox;
uniform bool u_GammaCorrect;

void main()
{
	vec3 envColor = texture(u_SkyBox, v_TexCoords).rgb;

	if (u_GammaCorrect) {
		envColor = envColor / (envColor + vec3(1.0));
		envColor = pow(envColor, vec3(1.0 / 2.2));
	}

	color = vec4(envColor, 1.0);
}