#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"
#include "00_Terrain.fx"

struct TerrainLODDesc
{
    float2 Distance;
    float2 TessFactor;

    float CellSpacing;
    float CellSpacingU;
    float CellSpacingV;
    float HeightScale;
};

cbuffer CB_TerrainLOD
{
    TerrainLODDesc TerrainLOD;
};

struct VertexOutput
{
    float4 Position : Position;
    float2 Uv : Uv;
};

VertexOutput VS(VertexOutput input)
{
    VertexOutput output;

    output.Position = input.Position;
    output.Uv = input.Uv;

    return output;
}

struct CHullOutput
{
    float Edge[4] : SV_TessFactor;
    float Inside[2] : SV_InsideTessFactor;
};


float CalcTessFactor(float3 position)
{
    float d = distance(position, ViewPosition());
    float4 s = saturate((d - TerrainLOD.Distance.y) / (TerrainLOD.Distance.x - TerrainLOD.Distance.y));

    return lerp(TerrainLOD.TessFactor.x, TerrainLOD.TessFactor.y, s);
}

CHullOutput CHS(InputPatch<VertexOutput, 4> input)
{
    //[0] [1]
    //[2] [3]

    float4 position[4];
    position[0] = WorldPosition(input[0].Position);
    position[1] = WorldPosition(input[1].Position);
    position[2] = WorldPosition(input[2].Position);
    position[3] = WorldPosition(input[3].Position);

    float3 e0 = (position[0] + position[2]).xyz * 0.5f; //Left
    float3 e1 = (position[0] + position[1]).xyz * 0.5f; //Top
    float3 e2 = (position[1] + position[3]).xyz * 0.5f; //Right
    float3 e3 = (position[2] + position[3]).xyz * 0.5f; //Bottom

    CHullOutput output;
    output.Edge[0] = CalcTessFactor(e0);
    output.Edge[1] = CalcTessFactor(e1);
    output.Edge[2] = CalcTessFactor(e2);
    output.Edge[3] = CalcTessFactor(e3);

    float3 center = (position[0].xyz + position[1].xyz + position[2].xyz + position[3].xyz) * 0.25f;
    output.Inside[0] = CalcTessFactor(center);
    output.Inside[1] = CalcTessFactor(center);

    return output;
}

struct HullOutput
{
    float4 Position : Position;
    float2 Uv : Uv;
};

[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("CHS")]
HullOutput HS(InputPatch<VertexOutput, 4> input, uint id : SV_OutputControlPointID)
{
    HullOutput output;

    output.Position = input[id].Position;
    output.Uv = input[id].Uv;

    return output;
}

struct DomainOutput
{
    float4 Position : SV_Position;
    float2 Uv : Uv;
};

[domain("quad")]
DomainOutput DS(CHullOutput input, const OutputPatch<HullOutput, 4> patch, float2 uv : SV_DomainLocation)
{
    DomainOutput output;

    float4 v1 = lerp(patch[0].Position, patch[1].Position, uv.x);
    float4 v2 = lerp(patch[2].Position, patch[3].Position, uv.x);
    float4 position = lerp(v1, v2, uv.y);

    float2 uv1 = lerp(patch[0].Uv, patch[1].Uv, uv.x);
    float2 uv2 = lerp(patch[2].Uv, patch[3].Uv, uv.x);
    float2 texCoord = lerp(uv1, uv2, uv.y);

    position.y = HeightMap.SampleLevel(LinearSampler, texCoord, 0).r * 255.0f / TerrainLOD.HeightScale;

    output.Position = float4(position.xyz, 1.0f);
    output.Position = WorldPosition(output.Position);
    output.Position = ViewProjection(output.Position);

    output.Uv = texCoord;

    return output;
}

float4 PS(DomainOutput input) : SV_Target
{
    float2 left =   input.Uv + float2(-TerrainLOD.CellSpacingU, 0.0f);
    float2 right =  input.Uv + float2(+TerrainLOD.CellSpacingU, 0.0f);
    float2 top =    input.Uv + float2(0.0f, -TerrainLOD.CellSpacingV);
    float2 bottom = input.Uv + float2(0.0f, +TerrainLOD.CellSpacingV);

    float leftY = HeightMap.Sample(LinearSampler, left).r * 255.0f / TerrainLOD.HeightScale;
    float rightY = HeightMap.Sample(LinearSampler, right).r * 255.0f / TerrainLOD.HeightScale;
    float topY = HeightMap.Sample(LinearSampler, top).r * 255.0f / TerrainLOD.HeightScale;
    float bottomY = HeightMap.Sample(LinearSampler, bottom).r * 255.0f / TerrainLOD.HeightScale;

    float3 T = normalize(float3(TerrainLOD.CellSpacing * 2.0f, rightY - leftY, 0.0f));
    float3 B = normalize(float3(0.0f, bottomY - topY, -TerrainLOD.CellSpacing * 2.0f));
    float3 N = normalize(cross(T, B));


    Material.Diffuse = BaseMap.Sample(LinearSampler, input.Uv);

    float3 map = NormalMap.Sample(LinearSampler, input.Uv).rgb;
    float3 coord = map.rgb * 2.0f - 1.0f;
    float3x3 TBN = float3x3(T, B, N);
    
    coord = mul(coord, TBN);
    
    return Material.Diffuse * saturate(dot(coord, -GlobalLight.Direction));

}

technique11 T0
{
    P_VTP(P0, VS, HS, DS, PS)
    P_RS_VTP(P1, FillMode_WireFrame, VS, HS, DS, PS)
}