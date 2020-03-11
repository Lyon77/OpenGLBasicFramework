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

uniform samplerCube u_EnviromentMap;

const float PI = 3.14159265359;

void main()
{
	vec3 normal = normalize(v_Pos);

	vec3 irradiance = vec3(0.0);

	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = cross(up, normal);
	up = cross(normal, right);

	float sampleDelta = 0.025;
	float nrSamples = 0.0;

	for (float phi = 0.0; phi < 2.0 * PI; phi += sampleDelta)
	{
		for (float theta = 0.0; theta < 0.5 * PI; theta += sampleDelta)
		{
			// spherical to cartesian (in tangent space)
			vec3 tangentSample = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			// convert to world space
			vec3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

			irradiance += texture(u_EnviromentMap, sampleVec).rgb * cos(theta) * sin(theta);
			nrSamples++;
		}
	}

	irradiance = PI * irradiance * (1.0 / float(nrSamples));

	color = vec4(irradiance, 1.0);
}