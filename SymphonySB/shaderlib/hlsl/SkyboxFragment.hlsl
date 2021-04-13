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
	return Texture.Sample(TextureSampler, input.texcoord);
}