Texture2D Texture: register(t0);
sampler TextureSampler: register(s0);

struct PS_INPUT
{
	float4 position: SV_POSITION;
	float3 color: COLOR;
	float2 texcoord: TEXCOORD;
};

float4 PSMain(PS_INPUT input) : SV_TARGET
{
	const float gamma = 2.2;
	float3 hdrColor = Texture.Sample(TextureSampler, input.texcoord).rgb;

	float mapX = hdrColor.x / (hdrColor.x + 1.0);
	float mapY = hdrColor.y / (hdrColor.y + 1.0);
	float mapZ = hdrColor.z / (hdrColor.z + 1.0);
	float3 map1 = float3(mapX, mapY, mapZ);

	map1.x = pow(abs(map1.x), 1.0 / gamma);
	map1.y = pow(abs(map1.y), 1.0 / gamma);
	map1.z = pow(abs(map1.z), 1.0 / gamma);

	return float4(map1, 1.0);
}