#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"

//-----------------------------------------------------------------------------
//Mesh, Model, Animation
//-----------------------------------------------------------------------------
float4 PS(MeshOutput input) : SV_Target
{
    return PS_Phong(input);
}


//-----------------------------------------------------------------------------
//Geometry Billboard
//-----------------------------------------------------------------------------
// -> Input From IA
struct VertexInput_Billboard
{
    float4 Position : Position;
    float2 Scale : Scale;
    uint MapIndex : MapIndex;
};

// -> Output to GS(VS -> GS)
struct VertexOutput_Billboard
{
    float4 Position : Position;
    float2 Scale : Scale;
    uint MapIndex : MapIndex;
};

// -> VS
VertexOutput_Billboard VS_Billboard(VertexInput_Billboard input)
{
    VertexOutput_Billboard output;

    output.Position = WorldPosition(input.Position);
    output.Scale = input.Scale;
    output.MapIndex = input.MapIndex;

    return output;
}

// -> GS Output (GS-> RS)
struct GeometryOutput_Billboard
{
    float4 Position : SV_Position;
    float2 Uv : Uv;
    uint MapIndex : MapIndex;
};

// -> GS(LookAtCamera)
[maxvertexcount(4)]
void GS_Billboard(point VertexOutput_Billboard input[1], inout TriangleStream<GeometryOutput_Billboard> stream)
{
    float3 up = float3(0, 1, 0);
    float3 forward = input[0].Position.xyz - ViewPosition();
    float3 right = normalize(cross(up, forward));

    float2 halfSize = input[0].Scale * 0.5f;

    float4 positions[4];
    positions[0] = float4(input[0].Position.xyz - halfSize.x * right - halfSize.y * up, 1);
    positions[1] = float4(input[0].Position.xyz - halfSize.x * right + halfSize.y * up, 1);
    positions[2] = float4(input[0].Position.xyz + halfSize.x * right - halfSize.y * up, 1);
    positions[3] = float4(input[0].Position.xyz + halfSize.x * right + halfSize.y * up, 1);

    float2 uvs[4] =  {float2(0, 1), float2(0, 0), float2(1, 1), float2(1, 0)};

    GeometryOutput_Billboard output;

    [unroll(4)]
    for (int i = 0; i < 4; i++)
    {
        output.Position = ViewProjection(positions[i]);
        output.Uv = uvs[i];
        output.MapIndex = input[0].MapIndex;

        stream.Append(output);
    }

}

// -> GS(CrossBillboard)
[maxvertexcount(8)]
void GS_Billboard_Cross(point VertexOutput_Billboard input[1], inout TriangleStream<GeometryOutput_Billboard> stream)
{
    float3 up = float3(0, 1, 0);
    float3 forward = float3(0, 0, 1);
    float3 right = normalize(cross(up, forward));

    float2 halfSize = input[0].Scale * 0.5f;

    float4 positions[8];
    positions[0] = float4(input[0].Position.xyz - halfSize.x * right - halfSize.y * up, 1);
    positions[1] = float4(input[0].Position.xyz - halfSize.x * right + halfSize.y * up, 1);
    positions[2] = float4(input[0].Position.xyz + halfSize.x * right - halfSize.y * up, 1);
    positions[3] = float4(input[0].Position.xyz + halfSize.x * right + halfSize.y * up, 1);

    positions[4] = float4(input[0].Position.xyz - halfSize.x * forward - halfSize.y * up, 1);
    positions[5] = float4(input[0].Position.xyz - halfSize.x * forward + halfSize.y * up, 1);
    positions[6] = float4(input[0].Position.xyz + halfSize.x * forward - halfSize.y * up, 1);
    positions[7] = float4(input[0].Position.xyz + halfSize.x * forward + halfSize.y * up, 1);

    float2 uvs[4] = { float2(0, 1), float2(0, 0), float2(1, 1), float2(1, 0) };

    GeometryOutput_Billboard output;

    [unroll(8)]
    for (int i = 0; i < 8; i++)
    {
        output.Position = ViewProjection(positions[i]);
        output.Uv = uvs[i % 4];
        output.MapIndex = input[0].MapIndex;

        if (i % 4 == 0)
            stream.RestartStrip();

        stream.Append(output);
    }

}

// -> PS
Texture2DArray BillboardMap;
float4 PS_Billboard(GeometryOutput_Billboard input) : SV_Target
{
    return BillboardMap.Sample(LinearSampler, float3(input.Uv, input.MapIndex));
}

technique11 T0
{
    P_VP(P0, VS_Mesh, PS)
    P_VP(P1, VS_Model, PS)
    P_VP(P2, VS_Animation, PS)

    P_BS_VGP(P3, AlphaBlend, VS_Billboard, GS_Billboard, PS_Billboard)
    P_BS_VGP(P4, AlphaBlend_AlphaToCoverageEnable, VS_Billboard, GS_Billboard, PS_Billboard)

    P_BS_VGP(P5, AlphaBlend, VS_Billboard, GS_Billboard_Cross, PS_Billboard)
    P_BS_VGP(P6, AlphaBlend_AlphaToCoverageEnable, VS_Billboard, GS_Billboard_Cross, PS_Billboard)

    P_RS_BS_VGP(P7, CullMode_None, AlphaBlend, VS_Billboard, GS_Billboard_Cross, PS_Billboard)
    P_RS_BS_VGP(P8, CullMode_None, AlphaBlend_AlphaToCoverageEnable, VS_Billboard, GS_Billboard_Cross, PS_Billboard)

    P_RS_DSS_BS_VGP(P9, CullMode_None, PixelDepthEnable_False, AlphaBlend, VS_Billboard, GS_Billboard_Cross, PS_Billboard)
}