#shader vertex
#version 330 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 texCoord;

out vec2 v_TexCoord;

void main()
{
	gl_Position = vec4(position.x, position.y, 0.0, 1.0);
	v_TexCoord = texCoord;
};

#shader fragment
#version 330 core

out vec4 color;

in vec2 v_TexCoord;

uniform sampler2D u_Texture;
uniform bool u_Horizontal;
uniform float weight[5] = float[](0.2270270270, 0.1945945946, 0.1216216216, 0.0540540541, 0.0162162162);

void main()
{
	vec2 texOffset = 1.0 / textureSize(u_Texture, 0);
	vec3 result = texture(u_Texture, v_TexCoord).rgb * weight[0];

	if (u_Horizontal)
	{
		for (int i = 1; i < 5; ++i)
		{
			result += texture(u_Texture, v_TexCoord + vec2(texOffset.x * i, 0.0)).rgb * weight[i];
			result += texture(u_Texture, v_TexCoord - vec2(texOffset.x * i, 0.0)).rgb * weight[i];
		}
	}
	else
	{
		for (int i = 1; i < 5; ++i)
		{
			result += texture(u_Texture, v_TexCoord + vec2(0.0, texOffset.y * i)).rgb * weight[i];
			result += texture(u_Texture, v_TexCoord - vec2(0.0, texOffset.y * i)).rgb * weight[i];
		}
	}

	color = vec4(result, 1.0);
};