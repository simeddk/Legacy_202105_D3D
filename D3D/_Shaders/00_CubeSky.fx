#include "00_Global.fx"
#include "00_Render.fx"

TextureCube SkyCubeMap;


float4 PS(MeshOutput input) : SV_Target
{
    return SkyCubeMap.Sample(LinearSampler, input.oPosition);
}


//------------------------------------------------------------------
//Pass
//------------------------------------------------------------------
technique11 T0
{
    P_RS_DSS_VP(P0, FrontCounterClockwise_True, DepthEnable_False, VS_Mesh, PS)
}