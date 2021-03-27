struct PS_INPUT
{
	float4 position: SV_POSITION;
	float3 color: COLOR;
	float2 texcoords: TEXCOORDS;
};

float4 PSMain(PS_INPUT input) : SV_TARGET
{
	return float4(input.color, 1.0);
}