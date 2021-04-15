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
	float4 LightDirection;
	float4 CameraPosition;
};

cbuffer MaterialRendererUniforms : register(b2)
{
	float3 Ambient;
	float3 Diffuse;
	float3 Specular;
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
	float3 camDir = normalize(input.position.xyz - CameraPosition.xyz);
	float3 lightDir = float3(LightDirection.xyz);

	// Ambient
	float ka = 0.1;
	float3 ambient_light = ka * Ambient;

	// Diffuse
	float kd = 0.7;
	float amount_diffuse_light = max(0.0, dot(lightDir, input.normals));
	float3 diffuse_light = amount_diffuse_light * Diffuse * kd;

	// SPECULAR LIGHT
	float ks = 0.5;
	float3 reflected_light = reflect(lightDir, input.normals);
	float amout_specular_light = pow(max(0.1, dot(reflected_light, camDir)), Shininess);
	float3 specular_light = ks * amout_specular_light * Specular;

	float3 final_light = ambient_light + diffuse_light + specular_light;

	float3 final_color = final_light + map1;

	return float4(final_color, 1.0);
}