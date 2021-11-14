#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"
#include "00_Terrain.fx"

float4 PS(MeshOutput input) : SV_Target
{
    return PS_Shadow(input.sPosition, PS_Phong(input));
        
}

technique11 T0
{
    //1Pass - Depth
    P_RS_VP(P0, FrontCounterClockwise_True, VS_Mesh_Depth, PS_Shadow_Depth)
    P_RS_VP(P1, FrontCounterClockwise_True, VS_Model_Depth, PS_Shadow_Depth)
    P_RS_VP(P2, FrontCounterClockwise_True, VS_Animation_Depth, PS_Shadow_Depth)
    P_RS_VP(P3, FrontCounterClockwise_True, VS_Terrain_Depth, PS_Shadow_Depth)
    
    //2 pass - MainRender
    P_RS_DSS_VP(P4, FrontCounterClockwise_True, DepthEnable_False, VS_Mesh, PS_Sky)
    P_VP(P5, VS_Mesh, PS)
    P_VP(P6, VS_Model, PS)
    P_VP(P7, VS_Animation, PS)
    P_VP(P8, VS_Terrain, PS_Terrain)
}