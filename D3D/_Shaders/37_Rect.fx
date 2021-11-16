#include "00_Global.fx"

//IA -> VS -> CHS -> HS -> TS -> DS -> GS -> SO -> RS -> PS -> OM
//CHS : 분할 기준(몇개를 쪼갤지)
//HS : 분할 위치
//TS : 실제 분할(프로그래밍 불가)
//DS : 분할된 정점의 WVP 변환


struct VertexOutput
{
    float4 Position : Position;
};

VertexOutput VS(VertexOutput input)
{
    VertexOutput output;
    output.Position = input.Position;

    return output;
}

struct CHullOutput
{
    float Edge[4] : SV_TessFactor;
    float Inside[2] : SV_InsideTessFactor;
};

uint Edge[4];
uint Inside[2];
CHullOutput CHS(InputPatch<VertexOutput, 4> input)
{
    CHullOutput output;
    output.Edge[0] = Edge[0];
    output.Edge[1] = Edge[1];
    output.Edge[2] = Edge[2];
    output.Edge[3] = Edge[3];

    output.Inside[0] = Inside[0];
    output.Inside[1] = Inside[1];

    return output;
}

struct HullOutput
{
    float4 Position : Position;
};

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("CHS")]
HullOutput HS(InputPatch<VertexOutput, 4> input, uint id : SV_OutputControlPointID)
{
    HullOutput output;
    output.Position = input[id].Position;

    return output;
}

struct DomainOutput
{
    float4 Position : SV_Position;
};

[domain("quad")]
DomainOutput DS(CHullOutput input, const OutputPatch<HullOutput, 4> patch, float2 uv : SV_DomainLocation)
{
    DomainOutput output;

    float4 v1 = lerp(patch[0].Position, patch[1].Position, 1 - uv.y);
    float4 v2 = lerp(patch[2].Position, patch[3].Position, 1 - uv.y);
    float4 position = lerp(v1, v2, uv.x);

    output.Position = float4(position.xyz, 1);

    return output;
}

float4 PS(DomainOutput input) : SV_Target
{
    return float4(0, 1, 0, 1);
}

technique11 T0
{
    P_RS_VTP(P0, FillMode_WireFrame, VS, HS, DS, PS)
}