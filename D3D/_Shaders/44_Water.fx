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
//Water
//-----------------------------------------------------------------------------
struct WaterDesc
{
    float4 RefractionColor;

    float2 NormalMapTile;
    float WaveTranlation;
    float WaveScale;

    float WaterShiness;
    float WaterAlpha;
    float WaveSpeed;
    float WaterPositionY;

    float TerrainWidth;
    float TerrainHeight;
    float WaterWidth;
    float WaterHeight;
};

cbuffer CB_Water
{
    WaterDesc Water;
};

struct VertexInput_Water
{
    float4 Position : Position;
    float2 Uv : Uv;
};

struct VertexOutput_Water
{
    float4 Position : SV_Position;
    float3 wPosition : Position1;
    float4 RefractionPosition : Position2;
    float4 ReflectionPosition : Position3;

    float2 Uv : Uv;

    float2 Normal1 : Uv1;
    float2 Normal2 : Uv2;
};

VertexOutput_Water VS_Water(VertexInput_Water input)
{
    VertexOutput_Water output;

    output.Position = WorldPosition(input.Position);
    output.wPosition = output.Position.xyz;
    output.Position = ViewProjection(output.Position);

    //Reflection WVP
    output.ReflectionPosition = WorldPosition(input.Position);
    output.ReflectionPosition = mul(output.ReflectionPosition, ReflectionView);
    output.ReflectionPosition = mul(output.ReflectionPosition, Projection);

    //Refraction WVP
    output.RefractionPosition = output.Position;

    output.Uv = input.Uv;

    output.Normal1 = input.Uv * Water.NormalMapTile.x;
    output.Normal2 = input.Uv * Water.NormalMapTile.y;

    return output;
}

float4 PS_Water(VertexOutput_Water input) : SV_Target
{
    //return float4(1, 0, 0, 1);

    input.Normal1.y += Water.WaveTranlation;
    input.Normal2.x += Water.WaveTranlation;

    float4 normalMap = NormalMap.Sample(LinearSampler, input.Normal1) * 2.0f - 1.0f;
    float4 normalMap2 = NormalMap.Sample(LinearSampler, input.Normal2) * 2.0f - 1.0f;
    float3 normal = normalMap.rgb + normalMap2.rgb;

    //Reflection
    float2 reflection = 0;
    reflection.x = input.ReflectionPosition.x / input.ReflectionPosition.w * 0.5f + 0.5f;
    reflection.y = -input.ReflectionPosition.y / input.ReflectionPosition.w * 0.5f + 0.5f;
    float4 reflectionColor = ReflectionMap.Sample(LinearSampler, reflection);
    //return float4(reflectionColor);

    //Refraction
    float2 refraction = 0;
    refraction.x = input.RefractionPosition.x / input.RefractionPosition.w * 0.5f + 0.5f;
    refraction.y = -input.RefractionPosition.y / input.RefractionPosition.w * 0.5f + 0.5f;
    refraction += normal.xy * Water.WaveScale;
    float4 refractionColor = RefractionMap.Sample(LinearSampler, refraction);
    //return refractionColor;

    //Specualr
    float3 light = GlobalLight.Direction;
    light.yz *= -1.0f;
    float3 E = normalize(ViewPosition() - input.wPosition);
    float3 R = normalize(reflect(light, normal));
    float specular = saturate(dot(R, E));
    float4 diffuse = 0;
    [flatten]
    if (specular > 0.0f)
    {
        specular = pow(specular, Water.WaterShiness);
        diffuse = saturate(diffuse + specular);
    }
    //return float4(diffuse.rgb, 1);

    //Fresnel
    float3 heightView = E.yyy;
    float r = 0.16667f;
    float fresnel = saturate(min(1, r + (1 - r) * pow(1 - dot(normal, heightView), 2)));
    diffuse += lerp(reflectionColor, refractionColor, fresnel);
    //return float4(diffuse.rgb, 1);

    float x = (Water.TerrainWidth / 2 - Water.WaterWidth / 2) / Water.TerrainWidth; //수면과 지면의 크기 차이 -> 비율화
    float y = (Water.TerrainHeight / 2 - Water.WaterHeight / 2) / Water.TerrainHeight;
    
    float u = input.Uv.x / Water.TerrainWidth * Water.WaterWidth; //전체 분에 1
    float v = input.Uv.y / Water.TerrainHeight * Water.WaterHeight;
    float2 uv = float2(x, y) + float2(u, v);
    
    float height = HeightMap.Sample(LinearSampler, uv /*float2(u, v)*/).r * 255.0f / 10.0f;
    height = saturate(Water.WaterPositionY * height / Water.WaterPositionY); //수면과 지면의 떨어진 정도
    
    float4 waterColor = WaterMap.Sample(LinearSampler, input.Uv * 0.2f /*reflection*/) * 2.0f; //워터맵이 너무 통짜네
    diffuse += (waterColor * height * 0.5f); //너무 밝군
    
    return float4(diffuse.rgb, 1 - (height * 0.75f));

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

    //1Pass - Reflection
    P_VP(P11, VS_Mesh_Reflection, PS)
    P_VP(P12, VS_Model_Reflection, PS)
    P_VP(P13, VS_Animation_Reflection, PS)
    P_VP(P14, VS_Terrain_Reflection, PS_Terrain)

    //1Pass - Reflection(Sky)
    P_VP(P15, VS_Scattering, PS_Scattering)
    P_DSS_VP(P16, DepthEnable_False, VS_Dome_Reflection, PS_Dome)
    P_DSS_BS_VP(P17, DepthEnable_False, AlphaBlend, VS_Moon_Reflection, PS_Moon)

    //2Pass - Reflection Surface
    P_BS_VP(P18, AlphaBlend, VS_Water, PS_Water)
}