#shader vertex
#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoord;

out vec2 v_TexCoord;

void main()
{
	//multiply position with projection matrix to account for window size
	gl_Position = vec4(position.x, position.y, 0.0, 1.0);
	v_TexCoord = texCoord;
};

#shader fragment
#version 330 core

out vec4 color;

in vec2 v_TexCoord;

uniform sampler2D u_Texture;
uniform float u_Exposure;

void main()
{
	vec3 hdrColor = texture(u_Texture, v_TexCoord).rgb;

	// reinhard tone mapping
	vec3 mapped = vec3(1.0) - exp(-hdrColor * u_Exposure);

	color = vec4(mapped, 1.0);
};