#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"

float4 PS(MeshOutput input) : SV_Target
{
    float4 color = PS_Phong(input);

    float4 wvp = input.wvpPosition_Sub;

    float u = wvp.x / wvp.w * 0.5f + 0.5f;
    float v = -wvp.y / wvp.w * 0.5f + 0.5f;
    float w = wvp.z / wvp.w;
    float3 uvw = float3(u, v, w);

    [flatten]
    if (saturate(uvw.x) == uvw.x && saturate(uvw.y) == uvw.y && saturate(uvw.z) == uvw.z)
    {
        float4 map = ProjectorMap.Sample(LinearSampler, uvw.xy);
        map.rgb += Projector.Color;
        color = lerp(color, map, map.a);
    }

    return color;

}

technique11 T0
{
    P_RS_DSS_VP(P0, FrontCounterClockwise_True, DepthEnable_False, VS_Mesh, PS_Sky)
    P_VP(P1, VS_Mesh_Projector, PS)
    P_VP(P2, VS_Model_Projector, PS)
    P_VP(P3, VS_Animation_Projector, PS)
}