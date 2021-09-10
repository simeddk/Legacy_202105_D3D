matrix World, View, Projection;

struct VertexInput
{
    float4 Position : Position;
};

struct VertexOutput
{
    float4 Position : SV_Position;
};

struct VertexOutput_HeightColor
{
    float4 Position : SV_Position;
    float4 Color : Color;
};

float4 GetHeightColor(float y)
{
    if (y > 20.0f)
        return float4(1, 0, 0, 1);

    if (y > 10.0f)
        return float4(0, 1, 0, 1);

    if (y > 5.0f)
        return float4(0, 0, 1, 1);

    return float4(1, 1, 1, 1);
}

VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    return output;
}

VertexOutput_HeightColor VS_HeightColor(VertexInput input)
{
    VertexOutput_HeightColor output;

    output.Position = mul(input.Position, World);
    output.Color = GetHeightColor(output.Position.y);
    
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    return output;
}

float4 PS(VertexOutput input) : SV_Target
{
    return float4(0, 1, 0, 1);
}

float4 PS_HeightColor(VertexOutput_HeightColor input) : SV_Target
{
    return input.Color;
}


RasterizerState Wireframe
{
    FillMode = WIREFRAME;
};

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
   
    pass P1
    {
        SetRasterizerState(Wireframe);

        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }


    pass P2
    {
        SetVertexShader(CompileShader(vs_5_0, VS_HeightColor()));
        SetPixelShader(CompileShader(ps_5_0, PS_HeightColor()));
    }
}