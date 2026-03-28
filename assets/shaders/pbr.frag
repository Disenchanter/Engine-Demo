#version 460 core
out vec4 FragColor;

in vec2 uv;
in vec3 normal;
in vec3 worldPosition;

uniform sampler2D albedoMap;
uniform float metallicFactor;
uniform float roughnessFactor;
uniform float aoFactor;

uniform vec3 ambientColor;
uniform vec3 cameraPosition;

struct DirectionalLight{
	vec3 direction;
	vec3 color;
};

uniform DirectionalLight directionalLight;

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return nom / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0;

	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

void main()
{
	vec3 albedo = texture(albedoMap, uv).rgb;
	float metallic = clamp(metallicFactor, 0.0, 1.0);
	float roughness = clamp(roughnessFactor, 0.04, 1.0);
	float ao = clamp(aoFactor, 0.0, 1.0);

	vec3 N = normalize(normal);
	vec3 V = normalize(cameraPosition - worldPosition);
	vec3 L = normalize(-directionalLight.direction);
	vec3 H = normalize(V + L);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	float NDF = DistributionGGX(N, H, roughness);
	float G = GeometrySmith(N, V, L, roughness);
	vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

	vec3 numerator = NDF * G * F;
	float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
	vec3 specular = numerator / denominator;

	vec3 kS = F;
	vec3 kD = vec3(1.0) - kS;
	kD *= 1.0 - metallic;

	float NdotL = max(dot(N, L), 0.0);
	vec3 radiance = directionalLight.color;
	vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;

	vec3 ambient = ambientColor * albedo * ao;
	vec3 color = ambient + Lo;

	FragColor = vec4(color, 1.0);
}
