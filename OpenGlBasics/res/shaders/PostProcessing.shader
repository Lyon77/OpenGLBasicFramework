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
uniform int u_Type = 1;

void main()
{
	//Setting up kernal information
	const float offset = 1.0 / 300.0;
	vec2 offsets[9] = vec2[](
		vec2(-offset, offset), // top-left
		vec2(0.0f, offset), // top-center
		vec2(offset, offset), // top-right
		vec2(-offset, 0.0f),   // center-left
		vec2(0.0f, 0.0f),   // center-center
		vec2(offset, 0.0f),   // center-right
		vec2(-offset, -offset), // bottom-left
		vec2(0.0f, -offset), // bottom-center
		vec2(offset, -offset)  // bottom-right    
	);

	if (u_Type == 1) { //Normal
		color = texture(u_Texture, v_TexCoord);
	}
	else if (u_Type == 2) { //Inverted
		color = vec4(vec3(1.0 - texture(u_Texture, v_TexCoord)), 1.0);
	}
	else if (u_Type == 3) { //GreyScale
		color = texture(u_Texture, v_TexCoord);
		float average = 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
		color = vec4(average, average, average, 1.0);
	}
	else if (u_Type == 4) { //Sharpen
		float kernel[9] = float[](
			-1, -1, -1,
			-1, 9, -1,
			-1, -1, -1
		);

		vec3 sampleTex[9];
		for (int i = 0; i < 9; i++)
		{
			sampleTex[i] = vec3(texture(u_Texture, v_TexCoord.st + offsets[i]));
		}
		vec3 col = vec3(0.0);
		for (int i = 0; i < 9; i++)
			col += sampleTex[i] * kernel[i];

		color = vec4(col, 1.0);
	}
	else if (u_Type == 5) { //Blur
		float kernel[9] = float[](
			1.0 / 16, 2.0 / 16, 1.0 / 16,
			2.0 / 16, 4.0 / 16, 2.0 / 16,
			1.0 / 16, 2.0 / 16, 1.0 / 16
		);

		vec3 sampleTex[9];
		for (int i = 0; i < 9; i++)
		{
			sampleTex[i] = vec3(texture(u_Texture, v_TexCoord.st + offsets[i]));
		}
		vec3 col = vec3(0.0);
		for (int i = 0; i < 9; i++)
			col += sampleTex[i] * kernel[i];

		color = vec4(col, 1.0);
	}
};