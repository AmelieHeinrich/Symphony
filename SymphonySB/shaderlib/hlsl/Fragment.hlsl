#include "shaderlib/hlsl/MatrixOperations.hlsl"

Texture2D Texture: register(t0);
sampler TextureSampler: register(s0);

struct PS_INPUT
{
	float4 position: SV_POSITION;
	float3 color: COLOR;
	float2 texcoord: TEXCOORD;
	float3 normals: NORMALS;
};

cbuffer RendererUniforms: register(b0)
{
	row_major float4x4 Model;
	row_major float4x4 View;
	row_major float4x4 Projection;
};

cbuffer LightRendererUniforms: register(b1)
{
	float3 CameraPosition;
	float3 LightDirection;
	float3 LightPosition;

	float3 LightAmbient;
	float3 LightDiffuse;
	float3 LightSpecular;

	float CutOff;
	float OuterCutOff;
	float Constant;
	float Linear;
	float Quadratic;
};

cbuffer MaterialRendererUniforms : register(b2)
{
	float3 Ambient;
	float3 Diffuse;
	float3 Specular;
	float3 Transmittance;
	float3 Emission;
	float Roughness;
	float Metallic;
	float Sheen;
	float IOR;
	float Dissolve;
	float ClearcoatThickness;
	float ClearcoatRoughness;
	float Shininess;
};

float4 PSMain(PS_INPUT input) : SV_TARGET
{
	// GAMMA CORRECTION

	const float gamma = 1.0;
	float3 hdrColor = Texture.Sample(TextureSampler, input.texcoord).rgb;

	float mapX = hdrColor.x / (hdrColor.x + 1.0);
	float mapY = hdrColor.y / (hdrColor.y + 1.0);
	float mapZ = hdrColor.z / (hdrColor.z + 1.0);
	float3 map1 = float3(mapX, mapY, mapZ);

	map1.x = pow(abs(map1.x), 1.0 / gamma);
	map1.y = pow(abs(map1.y), 1.0 / gamma);
	map1.z = pow(abs(map1.z), 1.0 / gamma);

	// Lighting Calculation
	float3 FragPos = float3(mul(Model, float4(input.position.xyz, 1.0)).xyz);
	float3 Normal = mul((float3x3)transpose(inverse(Model)), input.normals.xyz);
	float3 camDir = normalize(CameraPosition - FragPos);
	float3 lightDir = normalize(LightPosition - FragPos);

	// Ambient
	float ka = 0.1;
	float3 ambient_light = ka * Ambient * LightAmbient;

	// Diffuse
	float kd = 0.7;
	float amount_diffuse_light = max(0.1, dot(normalize(Normal), lightDir));
	float3 diffuse_light = amount_diffuse_light * Diffuse * kd * LightDiffuse;

	// SPECULAR LIGHT
	float ks = 0.5;
	float3 reflected_light = reflect(-lightDir, normalize(Normal));
	float amout_specular_light = pow(max(0.1, dot(camDir, reflected_light)), Shininess);
	float3 specular_light = ks * amout_specular_light * Specular * LightSpecular;

	// SPOTLIGHT (soft edges)
	float theta = dot(LightDirection, normalize(-LightDirection));
	float epsilon = (CutOff - OuterCutOff);
	float intensity = clamp((theta - OuterCutOff) / epsilon, 0.0, 1.0);
	diffuse_light *= intensity;
	specular_light *= intensity;

	// attenuation
	float distance = length(LightPosition - FragPos);
	float attenuation = 1.0 / (Constant + Linear * distance + Quadratic * pow(distance, 2));
	ambient_light *= attenuation;
	specular_light *= attenuation;

	float3 final_light = ambient_light + diffuse_light + specular_light;

	float3 final_color = final_light + map1;

	return float4(final_color, 1.0);
}