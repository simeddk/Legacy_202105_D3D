#include "00_Global.fx"

float3 LightDirection;
Texture2D DiffuseMap;
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
//Function
//------------------------------------------------------------------
VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    output.Position = WorldPosition(input.Position);
    output.Position = ViewProjection(output.Position);

    output.Normal = WorldNormal(input.Normal);

    output.Uv = input.Uv;

    return output;
}


float4 PS(VertexOutput input) : SV_Target
{
    float3 N = normalize(input.Normal);
    float3 L = LightDirection;
    float lambert = dot(N, -L);
   
    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.Uv);
    return diffuse * lambert;
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
        SetRasterizerState(FillMode_WireFrame);

        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}