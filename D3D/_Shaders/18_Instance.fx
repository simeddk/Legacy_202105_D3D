#include "00_Global.fx"
#include "00_Light.fx"

float3 LightDirection = float3(-1, -1, +1);

struct VertexInput
{
    float4 Position : Position;
    float2 Uv : Uv;
    float3 Normal : Normal;

    matrix Transform : Inst1_Transform;
    float4 Color : Inst2_Color;

    uint InstanceID : SV_InstanceID;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float2 Uv : Uv;
    float3 Normal : Normal;
    float4 Color : Color;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    World = input.Transform;

    input.Position = input.Position + float4((float) input.InstanceID * 2.0f, 0, 0, 1);

    output.Position = WorldPosition(input.Position);
    output.Position = ViewProjection(output.Position);

    output.Normal = input.Normal;
    output.Uv = input.Uv;

    return output;
}

VertexOutput VS_Color(VertexInput input)
{
    VertexOutput output;

    World = input.Transform;

    input.Position = input.Position + float4((float) input.InstanceID * 2.0f, 0, 0, 1);

    output.Position = WorldPosition(input.Position);
    output.Position = ViewProjection(output.Position);

    output.Normal = input.Normal;
    output.Uv = input.Uv;

    output.Color = input.Color;

    return output;
}


float4 PS(VertexOutput input) : SV_Target
{
    float3 normal = normalize(input.Normal);
    float lambert = dot(normal, -LightDirection);
   
    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.Uv);
    return diffuse * lambert;
}

float4 PS_Color(VertexOutput input) : SV_Target
{
    float3 normal = normalize(input.Normal);
    float lambert = dot(normal, -LightDirection);
   
    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.Uv);
    return diffuse * lambert + input.Color;
}

technique11 T0
{
    P_VP(P0, VS, PS)
    P_VP(P1, VS_Color, PS_Color)
}