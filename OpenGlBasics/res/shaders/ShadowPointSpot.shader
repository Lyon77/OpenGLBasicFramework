#shader vertex
#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 u_Model;

void main()
{
	gl_Position = u_Model * vec4(position, 1.0);
};

#shader geometry
#version 330 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

uniform mat4 u_ShadowMatrices[6];

out vec4 g_Pos;

void main()
{
	for (int face = 0; face < 6; ++face)
	{
		gl_Layer = face; // built-in variable that specifies to which face we render.
		for (int i = 0; i < 3; ++i) // for each triangle's vertices
		{
			g_Pos = gl_in[i].gl_Position;
			gl_Position = u_ShadowMatrices[face] * g_Pos;
			EmitVertex();
		}
		EndPrimitive();
	}
}

#shader fragment
#version 330 core

in vec4 g_Pos;

uniform vec3 u_LightPos;
uniform float u_FarPlane;

void main()
{
	// get distance between fragment and light source
	float lightDistance = length(g_Pos.xyz - u_LightPos);

	// map to [0;1] range by dividing by far_plane
	lightDistance = lightDistance / u_FarPlane;

	// write this as modified depth
	gl_FragDepth = lightDistance;
};