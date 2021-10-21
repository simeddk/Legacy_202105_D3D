#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"


float4 PS(MeshOutput input) : SV_Target
{
    NormalMapping(input.Uv, input.Normal, input.Tangent);

    Texture(Material.Diffuse, DiffuseMap, input.Uv);
    Texture(Material.Specular, SpecularMap, input.Uv);

    MaterialDesc output;
    ComputeLight(output, input.Normal, input.wPosition);
    
    return float4(MaterialToColor(output), 1.0f) + input.Color;
}


uint Mode = 0;
float4 PS_Test(MeshOutput input) : SV_Target
{
    Material.Diffuse = float4(1, 1, 1, 1);

    if (Mode == 0)
    {

    }
    else if(Mode == 1)
    {
        NormalMapping(input.Uv, input.Normal, input.Tangent);
    }
    else if (Mode == 2)
    {
        Texture(Material.Diffuse, DiffuseMap, input.Uv);
    }
    else if(Mode == 3)
    {
        Texture(Material.Diffuse, DiffuseMap, input.Uv);
        NormalMapping(input.Uv, input.Normal, input.Tangent);
    }

    
    Texture(Material.Specular, SpecularMap, input.Uv);

    MaterialDesc output;
    ComputeLight(output, input.Normal, input.wPosition);
    
    return float4(MaterialToColor(output), 1.0f) + input.Color;
}

technique11 T0
{
    P_VP(P0, VS_Mesh, PS)
    P_VP(P1, VS_Model, PS)
    P_VP(P2, VS_Animation, PS)

    P_VP(P3, VS_Mesh, PS_Test)
    P_VP(P4, VS_Model, PS_Test)
    P_VP(P5, VS_Animation, PS_Test)
}