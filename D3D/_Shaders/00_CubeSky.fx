#include "00_Global.fx"

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
//Function
//------------------------------------------------------------------
VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    output.oPosition = input.Position.xyz;

    output.Position = WorldPosition(input.Position);
    output.Position = ViewProjection(output.Position);
  
    return output;
}


float4 PS(VertexOutput input) : SV_Target
{
   
    return SkyCubeMap.Sample(LinearSampler, input.oPosition);
}


//------------------------------------------------------------------
//Pass
//------------------------------------------------------------------
technique11 T0
{
    P_VP(P0, VS, PS)
    P_RS_VP(P1, FrontCounterClockwise_True, VS, PS)
    P_RS_DSS_VP(P2, FrontCounterClockwise_True, DepthEnable_False, VS, PS)
}