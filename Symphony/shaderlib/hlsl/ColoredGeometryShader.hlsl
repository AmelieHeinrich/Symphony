struct VertexShader_Input
{
    float4 position: POSITION;
    float3 color: COLOR;
};

struct VertexShader_Output
{
    float4 position: SV_POSITION;
	float3 color: COLOR;
};

struct PixelShader_Input
{
    float4 position: SV_POSITION;
	float3 color: COLOR;
};

cbuffer ShaderUniformBuffer : register(b0)
{
    row_major float4x4 ModelViewProjectionMatrix;
};

VertexShader_Output VertexShaderMain(VertexShader_Input input)
{
    VertexShader_Output output = (VertexShader_Output)0;

    output.position = mul(input.position, ModelViewProjectionMatrix);
    output.color = input.color;

    return output;
}

float4 PixelShaderMain(PixelShader_Input input) : SV_Target
{
    return float4(input.color, 1.0);
}