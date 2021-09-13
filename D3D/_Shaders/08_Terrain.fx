matrix World, View, Projection;
float3 LightDirection;
Texture2D BaseMap;
uint Albedo = 0;

//------------------------------------------------------------------
//Datas
//------------------------------------------------------------------
struct VertexInput
{
    float4 Position : Position;
    float2 Uv : Uv;
    float3 Normal : Nomral;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float2 Uv : Uv;
    float3 Normal : Nomral;
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

//------------------------------------------------------------------
//Function
//------------------------------------------------------------------
VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.Normal = mul(input.Normal, (float3x3)World);

    output.Uv = input.Uv;

    return output;
}


float4 PS(VertexOutput input) : SV_Target
{
    float3 N = normalize(input.Normal);
    float3 L = LightDirection;
    float lambert = dot(N, -L);

    float4 baseMap = BaseMap.Sample(LinearWarpSampler, input.Uv);

    if (Albedo == 1)
        return baseMap;

    return baseMap * lambert;
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
        SetRasterizerState(Wireframe);

        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}