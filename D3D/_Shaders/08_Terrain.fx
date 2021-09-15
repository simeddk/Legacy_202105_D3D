#include "00_Global.fx"

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

    float4 baseMap = BaseMap.Sample(LinearSampler, input.Uv);

    if (Albedo == 1)
        return baseMap;

    return baseMap * lambert;
}


//------------------------------------------------------------------
//Pass
//------------------------------------------------------------------
technique11 T0
{
    P_VP(P0, VS, PS)
    P_RS_VP(P1, FillMode_WireFrame, VS, PS)

}