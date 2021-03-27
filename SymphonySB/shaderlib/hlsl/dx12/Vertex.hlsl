struct VS_INPUT
{
	float4 position: POSITION;
	float3 color: COLOR;
	float2 texcoords: TEXCOORDS;
};

struct VS_OUTPUT
{
	float4 position: SV_POSITION;
	float3 color: COLOR;
	float2 texcoords: TEXCOORDS;
};

VS_OUTPUT VSMain(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	output.position = input.position;
	output.color = input.color;
	output.texcoords = input.texcoords;

	return output;
}