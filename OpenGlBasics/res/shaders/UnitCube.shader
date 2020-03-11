#shader vertex
#version 330 core

layout(location = 0) in vec3 position;

out vec3 v_Pos;

uniform mat4 u_VP;

void main()
{
	v_Pos = position;
	gl_Position = u_VP * vec4(position, 1.0);
};

#shader fragment
#version 330 core

out vec4 color;
in vec3 v_Pos;

uniform sampler2D u_EquirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
	uv *= invAtan;
	uv += 0.5;
	return uv;
}

void main()
{
	vec2 uv = SampleSphericalMap(normalize(v_Pos));

	color = vec4(texture(u_EquirectangularMap, uv).rgb, 1.0);
}