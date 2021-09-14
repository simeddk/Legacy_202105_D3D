matrix World, View, Projection;
TextureCube SkyCubeMap;

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
    float3 oPosition : Position1;
};

//------------------------------------------------------------------
//State
//------------------------------------------------------------------
SamplerState LinearWarpSampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = WRAP;
    AddressV = WRAP;
};

RasterizerState Wireframe
{
    FillMode = WireFrame;
};

RasterizerState CCW
{
    FrontCounterClockwise = true;
};

DepthStencilState DepthDisable
{
    DepthEnable = false;
};

//------------------------------------------------------------------
//Function
//------------------------------------------------------------------
VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    output.oPosition = input.Position.xyz;

    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
  
    return output;
}


float4 PS(VertexOutput input) : SV_Target
{
   
    return SkyCubeMap.Sample(LinearWarpSampler, input.oPosition);
}


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
        SetRasterizerState(CCW);

        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }

    pass P2
    {
        SetRasterizerState(CCW);
        SetDepthStencilState(DepthDisable, 1);

        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}