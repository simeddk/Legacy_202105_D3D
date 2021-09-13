matrix World, View, Projection;
float3 LightDirection;

//------------------------------------------------------------------
//Datas
//------------------------------------------------------------------
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

struct VertexInput_Lambert
{
    float4 Position : Position;
    float3 Normal : Nomral;
};

struct VertexOutput_Lambert
{
    float4 Position : SV_Position;
    float4 Color : Color;
    float3 Normal : Nomral;
};

//------------------------------------------------------------------
//Function
//------------------------------------------------------------------
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

VertexOutput_Lambert VS_Lambert(VertexInput_Lambert input)
{
    VertexOutput_Lambert output;

    output.Position = mul(input.Position, World);
    output.Color = GetHeightColor(output.Position.y);
    
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = mul(input.Normal, (float3x3)World);

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


float4 PS_Lambert(VertexOutput_Lambert input) : SV_Target
{
    return input.Color * dot(normalize(input.Normal), -LightDirection);
}

float4 PS_HalfLambert(VertexOutput_Lambert input) : SV_Target
{
    return input.Color * (dot(input.Normal, -LightDirection) * 0.5f + 0.5f);
}




RasterizerState Wireframe
{
    FillMode = WIREFRAME;
};

//------------------------------------------------------------------
//Pass
//------------------------------------------------------------------
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

    pass P3
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Lambert()));
        SetPixelShader(CompileShader(ps_5_0, PS_Lambert()));
    }

    pass P4
    {
        SetVertexShader(CompileShader(vs_5_0, VS_Lambert()));
        SetPixelShader(CompileShader(ps_5_0, PS_HalfLambert()));
    }
}