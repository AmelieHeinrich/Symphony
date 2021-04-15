struct VS_INPUT
{
	float4 position: POSITION;
	float3 color: COLOR;
	float2 texcoord: TEXCOORD;
	float3 normals: NORMALS;
};

struct VS_OUTPUT
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

VS_OUTPUT VSMain(VS_INPUT input)
{
	VS_OUTPUT output = (VS_OUTPUT)0;

	float3 FragPos = float3(mul(Model, float4(input.position.xyz, 1.0)).xyz);

	output.position = float4(FragPos.xyz, 1.0);
	output.position = mul(output.position, Projection);
	output.position = mul(output.position, View);
	output.color = input.color;
	output.texcoord = input.texcoord;
	output.normals = input.normals;

	return output;
}