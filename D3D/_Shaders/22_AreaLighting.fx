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
};

// -> Output to GS(VS -> GS)
struct VertexOutput_Billboard
{
    float4 Position : Position;
    float2 Scale : Scale;
};

// -> VS
VertexOutput_Billboard VS_Billboard(VertexInput_Billboard input)
{
    VertexOutput_Billboard output;

    output.Position = WorldPosition(input.Position);
    output.Scale = input.Scale;

    return output;
}

// -> GS Output (GS-> RS)
struct GeometryOutput_Billboard
{
    float4 Position : SV_Position;
    float2 Uv : Uv;
};

// -> GS
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

        stream.Append(output);
    }

}

// -> PS
float4 PS_Billboard(GeometryOutput_Billboard input) : SV_Target
{
    return DiffuseMap.Sample(LinearSampler, input.Uv);
}

technique11 T0
{
    P_VP(P0, VS_Mesh, PS)
    P_VP(P1, VS_Model, PS)
    P_VP(P2, VS_Animation, PS)

    P_BS_VGP(P3, AlphaBlend, VS_Billboard, GS_Billboard, PS_Billboard)
}