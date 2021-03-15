struct VertexShader_Input
{
    float4 position: POSITION;
    float3 color: COLOR;
    float2 texcoord: TEXCOORD;
};

struct VertexShader_Output
{
    float4 position: SV_POSITION;
	float3 color: COLOR;
    float2 texcoord: TEXCOORD;
};

Texture2D Texture: register(t0);
sampler TextureSampler: register(s0);

struct PixelShader_Input
{
    float4 position: SV_POSITION;
	float3 color: COLOR;
    float2 texcoord: TEXCOORD;
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
    output.texcoord = input.texcoord;

    return output;
}

float4 PixelShaderMain(PixelShader_Input input) : SV_Target
{
    return Texture.Sample(TextureSampler, input.texcoord);
}