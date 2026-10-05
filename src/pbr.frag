#version 450 core

layout (location = 0) out vec4 fColour;
// Based off guide by https://learnopengl.com/PBR/Lighting

in vec3 Normal;
in vec3 FragPosWorldSpace;
in vec2 TexCoord;
in vec3 Tangent;

struct Light {
	vec3 position;
	vec3 direction;
	vec3 colour;
	int type; // 0 = directional, 1 = point, 2 = spot
	int shadowIndex; // -1 for no shadows
};

#define MAX_SHADOWS 8
uniform sampler2D shadowMaps[MAX_SHADOWS];

uniform mat4 lightSpaceMatrices[MAX_SHADOWS];

#define MAX_LIGHTS 8
uniform Light lights[MAX_LIGHTS];
uniform int numLights;

uniform vec3 camPos;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D aoMap;

// Procedual terrain textures
uniform sampler2D grassAlbedo;
uniform sampler2D rockAlbedo;
uniform sampler2D snowAlbedo;

uniform sampler2D grassNormal;
uniform sampler2D rockNormal;
uniform sampler2D snowNormal;

uniform sampler2D grassRough;
uniform sampler2D rockRough;
uniform sampler2D snowRough;

uniform sampler2D grassAo;
uniform sampler2D rockAo;
uniform sampler2D snowAo;

uniform bool hasNormal;
uniform bool hasMetal;
uniform bool hasRough;
uniform bool hasAo;

uniform bool isProcedural;

uniform float transparency;

const float PI = 3.14159265359;

vec3 fresnelSchlick(vec3 V, vec3 H, vec3 F0)
{
	return F0 + (vec3(1.0) - F0) * pow(1.0 - max(dot(V, H), 0.0), 5.0);
}

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return a2 / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r * r) / 8.0;

	float denom = NdotV * (1.0 - k) + k;

	return NdotV / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}

vec3 getNormal(vec3 tangentNormal)
{
	// convert to [0,1] -> [-1.1]
	tangentNormal = tangentNormal * 2.0 - 1.0;
	
	// Construct TBN matrix
	vec3 T = normalize(Tangent);
	vec3 N = normalize(Normal);
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);

	mat3 TBN = mat3(T, B, N);

	return normalize(TBN * tangentNormal);
}

float calculateShadow(int shadowIndex, mat4 lightMatrix, vec3 normal, vec3 lightDir)
{
	vec4 fragLightSpace = lightMatrix * vec4(FragPosWorldSpace, 1.0);
	vec3 ndc = fragLightSpace.xyz / fragLightSpace.w;
	vec3 ss = (ndc+1) * 0.5;
	
	float fragDepth = ss.z;

	float litDepth = texture(shadowMaps[shadowIndex], ss.xy).r;

	float bias = max(0.005 * (1.0 - dot(normal, lightDir)), 0.001);

	float shadow = fragDepth > (litDepth + bias) ? 1.0 : 0.0;

	if (fragDepth > 1)
		shadow = 0.f;

	return shadow;
}

vec3 calculatePBRIllumination(vec3 N, vec3 V, vec3 L, vec3 H, vec3 lightColour, vec3 albedo, float roughness, float metallic, vec3 F0)
{
		vec3 Ks = fresnelSchlick(V, H, F0);
		vec3 Kd = (vec3(1.0) - Ks) * (1.0 - metallic);

		vec3 lambert = albedo / PI;

		vec3 cookTorranceNumerator = DistributionGGX(N, H, roughness) * GeometrySmith(N, V, L, roughness) * Ks;
		float cookTorranceDenominator = 4.0 * max(dot(V, N), 0.0) * max(dot(L, N), 0.0);
		cookTorranceDenominator = max(cookTorranceDenominator, 0.000001);
		vec3 cookTorrance = cookTorranceNumerator / cookTorranceDenominator;

		vec3 brdf = Kd * lambert + cookTorrance;

		return brdf * lightColour * max(dot(L, N), 0.0);

}

vec3 calculateDirectionalLight(Light light, vec3 N, vec3 V, vec3 albedo, float metallic, float roughness, vec3 F0)
{
	vec3 L = normalize(light.position);
	vec3 H = normalize(V + L);
	vec3 lightValue = calculatePBRIllumination(N, V, L, H, light.colour, albedo, roughness, metallic, F0);
	float shadow = 0.0;
	if (light.shadowIndex >= 0)
		shadow = calculateShadow(light.shadowIndex, lightSpaceMatrices[light.shadowIndex], N, -light.direction);

	return (1.0 - shadow) * lightValue;
}

vec3 calculatePointLight(Light light, vec3 N, vec3 V, vec3 albedo, float metallic, float roughness, vec3 F0)
{
	vec3 L = normalize(light.position - FragPosWorldSpace);
	vec3 H = normalize(V + L);

	vec3 lightValue = calculatePBRIllumination(N, V, L, H, light.colour, albedo, roughness, metallic, F0);
	float dist = length(light.position - FragPosWorldSpace);
	float attenuation = 1.0f / max(1.0 + 0.05 * dist + 0.002 * pow(dist, 2), 0.00001);

	return attenuation * lightValue;
}

vec3 calculateSpotLight(Light light, vec3 N, vec3 V, vec3 albedo, float metallic, float roughness, vec3 F0)
{
	float phi = cos(radians(12.5f));
	vec3 NspotDir = normalize(light.direction);
	vec3 Nfrom_light = normalize(FragPosWorldSpace - light.position);
	float theta = dot(Nfrom_light, NspotDir);
	
	float shadow = 0.0;
	if (light.shadowIndex >= 0)
		shadow = calculateShadow(light.shadowIndex, lightSpaceMatrices[light.shadowIndex], N, -Nfrom_light);

	if (theta > phi) {
		vec3 lightValue = calculatePointLight(light, N, V, albedo, metallic, roughness, F0);
		return (1.0 - shadow) * lightValue;
	}
	return vec3(0.0);
		
}


void main()
{
	float metallic = 0.0;
	if (hasMetal)
		metallic = texture(metallicMap, TexCoord).r;

	float roughness = 0.5;
	if (hasRough)
		roughness = texture(roughnessMap, TexCoord).r;

	float ao = 1.0;
	if (hasAo)
		ao = texture(aoMap, TexCoord).r;

	vec3 N = Normal;
	if (hasNormal)
		N = getNormal(texture(normalMap, TexCoord).rgb);
	vec3 albedo;

	if (isProcedural) {
		float height = FragPosWorldSpace.y;

		vec3 grassColour = texture(grassAlbedo, TexCoord).rgb;
		vec3 rockColour = texture(rockAlbedo, TexCoord).rgb;
		vec3 snowColour = texture(snowAlbedo, TexCoord).rgb;

		vec3 grassN = texture(grassNormal, TexCoord).rgb;
		vec3 rockN = texture(rockNormal, TexCoord).rgb;
		vec3 snowN = texture(snowNormal, TexCoord).rgb;

		float grassR = texture(grassRough, TexCoord).r;
		float rockR = texture(rockRough, TexCoord).r;
		float snowR = texture(snowRough, TexCoord).r;

		float grassAo = texture(grassAo, TexCoord).r;
		float rockAo = texture(rockAo, TexCoord).r;
		float snowAo = texture(snowAo, TexCoord).r;


		if (height < 5.0) {
			albedo = grassColour;
			roughness = grassR;
			ao = grassAo;
			N = getNormal(grassN);
		} else if (height < 10.0) {
			float t = (height - 5.0) / 5.0;
			albedo = mix(grassColour, rockColour, t);
			roughness = mix(grassR, rockR, t);
			ao = mix(grassAo, rockAo, t);
			N = getNormal(mix(grassN, rockN, t));
		} else if (height < 20.0) {
			albedo = rockColour;
			roughness = rockR;
			ao = rockAo;
			N = getNormal(rockN);
		} else if (height < 25.0) {
			float t = (height - 20.0) / 5.0;
			albedo = mix(rockColour, snowColour, t);
			roughness = mix(rockR, snowR, t);
			ao = mix(rockAo, snowAo, t);
			N = getNormal(mix(rockN, snowN, t));
		} else {
			albedo = snowColour;
			roughness = snowR;
			ao = snowAo;
			N = getNormal(snowN);
		}

		albedo = pow(albedo, vec3(2.2));

	} else {
		albedo = pow(texture(albedoMap, TexCoord).rgb, vec3(2.2)); // sRGB to linear
	}


	vec3 V = normalize(camPos - FragPosWorldSpace);
	vec3 F0 = mix(vec3(0.04), albedo, metallic);

	vec3 Lo = vec3(0.0);

	for (int i = 0; i < numLights; i++) 
	{
		if (lights[i].type == 0) { // directional light
			Lo += calculateDirectionalLight(lights[i], N, V, albedo, roughness, metallic, F0);
		}
		if (lights[i].type == 1) { // Point
			Lo += calculatePointLight(lights[i], N, V, albedo, roughness, metallic, F0);
		}
		if (lights[i].type == 2) { // Spot
			Lo += calculateSpotLight(lights[i], N, V, albedo, roughness, metallic, F0);
		}
	}

	vec3 ambient = vec3(0.05) * albedo * ao;
	vec3 colour = ambient + Lo;
	
	// gamma correction
	colour = colour / (colour + vec3(1.0));
	colour = pow(colour, vec3(1.0/2.2));

	//fColour = vec4(colour, 0.1);
	fColour = vec4(colour, 1.0 - transparency);
}
