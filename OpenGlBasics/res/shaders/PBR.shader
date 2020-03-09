#shader vertex
#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTexCoords;
layout(location = 2) in vec3 aNormal;

out vec2 v_TexCoord;
out vec3 v_WorldPos;
out vec3 v_Normal;

uniform mat4 u_MVP;
uniform mat4 u_Model;

void main()
{
	//multiply position with projection matrix to account for window size
	gl_Position = u_MVP * vec4(aPos, 1.0);
	v_TexCoord = aTexCoords;
	v_WorldPos = vec3(u_Model * vec4(aPos, 1.0));
	v_Normal = mat3(u_Model) * aNormal;
};

#shader fragment
#version 330 core

out vec4 color;

in vec2 v_TexCoord;
in vec3 v_WorldPos;
in vec3 v_Normal;

uniform vec3 u_CameraPos;

uniform vec3  u_Albedo;
uniform float u_Metallic;
uniform float u_Roughness;
uniform float u_AO;

uniform vec3 u_LightPositions[4];
uniform vec3 u_LightColors[4];

const float PI = 3.14159265359;

// the normal distribution function approximating the relative surface area of microfacets exactly aligned to the (halfway) vector h
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float num = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return num / denom;
}
 // The geometry function statistically approximates the relative surface area where its micro surface-details overshadow each other causing light rays to be occluded
float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float num = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return num / denom;
}
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

// Describes the ratio of light that gets reflected over the light that gets refracted
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
	vec3 N = normalize(v_Normal);                             // Normal vector
	vec3 V = normalize(u_CameraPos - v_WorldPos);             // View vector

	// calculate reflectance at normal incidence
	// if dia-electric use F0 of 0.04 and if it's a metal, use the albedo color as F0 
	vec3 F0 = vec3(0.04);
	F0 = mix(F0, u_Albedo, u_Metallic);

	// reflectance equation
	vec3 Lo = vec3(0.0);
	for (int i = 0; i < 4; i++) {
		vec3 L = normalize(u_LightPositions[i] - v_WorldPos); // Light Vector
		vec3 H = normalize(V + L);                            // Halfway Vector

		float distance = length(u_LightPositions[i] - v_WorldPos);
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = u_LightColors[i] * attenuation;

		float NDF = DistributionGGX(N, H, u_Roughness);
		float G = GeometrySmith(N, V, L, u_Roughness);
		vec3 F = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);

		// The Cook-Torrance BRDF
		vec3 numerator = NDF * G * F;
		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
		vec3 specular = numerator / max(denominator, 0.001);

		// Calculate amount reflected (S) and refracted (D)
		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;

		kD *= 1.0 - u_Metallic;

		const float PI = 3.14159265359;

		float NdotL = max(dot(N, L), 0.0);
		Lo += (kD * u_Albedo / PI + specular) * radiance * NdotL;
	}

	vec3 ambient = vec3(0.03) * u_Albedo * u_AO;
	vec3 finalColor = ambient + Lo;

	// Gamma Correction
	finalColor = finalColor / (finalColor + vec3(1.0));
	finalColor = pow(finalColor, vec3(1.0 / 2.2));
	color = vec4(finalColor, 1.0);
};