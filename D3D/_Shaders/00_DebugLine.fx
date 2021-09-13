matrix World, View, Projection;

struct VertexInput
{
    float4 Position : Position;
    float4 Color : Color;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float4 Color : Color;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Color = input.Color;

    return output;
}

float4 PS(VertexOutput input) : SV_Target
{
    return input.Color;
}

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}