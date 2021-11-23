#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"
#include "00_Terrain.fx"
#include "00_Sky.fx"

//-----------------------------------------------------------------------------
//Mesh, Model, Animation
//-----------------------------------------------------------------------------
float4 PS(MeshOutput input) : SV_Target
{
    return CalcualteFogColor(PS_Shadow(input.sPosition, PS_Phong(input)), input.wPosition);
}

//-----------------------------------------------------------------------------
//Refraction
//-----------------------------------------------------------------------------
float4 PS_Refraction(MeshOutput input) : SV_Target
{
    float2 refraction;
    refraction.x = input.wvpPosition.x / input.wvpPosition.w * 0.5f + 0.5f;
    refraction.y = -input.wvpPosition.y / input.wvpPosition.w * 0.5f + 0.5f;

    input.Uv.x += Time * 0.05f;
    input.Uv.y += Time * 0.1f;

    float3 normal = NormalMap.Sample(LinearSampler, input.Uv).rgb * 2.0f - 1.0f;
    refraction += normal.xy * 0.05f;

    float4 color = RefractionMap.Sample(LinearSampler, refraction);
    color.b *= 1.2f;

    return float4(color.rgb, 1);
}



technique11 T0
{
    //1Pass - Depth
    P_RS_VP(P0, FrontCounterClockwise_True, VS_Mesh_Depth, PS_Shadow_Depth)
    P_RS_VP(P1, FrontCounterClockwise_True, VS_Model_Depth, PS_Shadow_Depth)
    P_RS_VP(P2, FrontCounterClockwise_True, VS_Animation_Depth, PS_Shadow_Depth)
    P_RS_VP(P3, FrontCounterClockwise_True, VS_Terrain_Depth, PS_Shadow_Depth)

    //2Pass - MainRender
    P_VP(P4, VS_Mesh, PS)
    P_VP(P5, VS_Model, PS)
    P_VP(P6, VS_Animation, PS)
    P_VP(P7, VS_Terrain, PS_Terrain)

    //2Pass - Sky
    P_VP(P8, VS_Scattering, PS_Scattering)
    P_DSS_VP(P9, DepthEnable_False, VS_Dome, PS_Dome)
    P_DSS_BS_VP(P10, DepthEnable_False, AlphaBlend, VS_Moon, PS_Moon)

    //2Pass - Refraction
    P_BS_VP(P11, AlphaBlend, VS_Mesh, PS_Refraction)
}