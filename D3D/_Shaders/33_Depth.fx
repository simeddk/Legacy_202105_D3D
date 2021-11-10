#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"

float4 PS(MeshOutput input) : SV_Target
{
    float3 ndc = input.wvpPosition.xyz / input.wvpPosition.w;
    
    
    float n = 0.1f;
    float f = 1000.0f;
    
    float depth = ndc.z;
    //float depth = n * 2.0f / (f + n - ndc.z * (f - n));

    float4 color = float4(1, 1, 1, 1);
    
    if (depth < 0.1f)
        color = float4(1, 0, 0, 1);
    else if (depth < 0.3f)
        color = float4(0, 1, 0, 1);
    else if (depth < 0.5f)
        color = float4(0, 0, 1, 1);

    return color;
}

technique11 T0
{
    P_VP(P0, VS_Mesh, PS)
    P_VP(P1, VS_Model, PS)
    P_VP(P2, VS_Animation, PS)
}