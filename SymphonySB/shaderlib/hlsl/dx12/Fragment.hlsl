Texture2D t1 : register(t0);
SamplerState s1 : register(s0);

struct PS_INPUT
{
	float4 position: SV_POSITION;
	float3 color: COLOR;
	float2 texcoords: TEXCOORDS;
};

float4 PSMain(PS_INPUT input) : SV_TARGET
{
	return t1.Sample(s1, input.texcoords);
}