#include "00_Global.fx"
#include "00_Light.fx"

cbuffer CB_Snow
{
    float4 Color;

    float3 Velocity;
    float DrawDistance;

    float3 Origin;
    float Turbulence;

    float3 Extent;
};

struct VertexInput
{
    float4 Position : Position;
    float2 Uv : Uv;
    float Scale : Scale;
    float2 Random : Random;
}; 

struct VertexOutput
{
    float4 Position : SV_Position;
    float2 Uv : Uv;
    float Alpha : Alpha;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    input.Position.x += cos(Time - input.Random.x) * Turbulence;
    input.Position.z += cos(Time - input.Random.y) * Turbulence;

    float3 velocity = Velocity * Time;
    input.Position.xyz = Origin + (Extent + (input.Position.xyz + velocity) % Extent) % Extent - (Extent * 0.5f);

    float4 position = input.Position;
    
    float3 up = float3(0, 1, 0);
    float3 forward = normalize(position.xyz - ViewPosition());
    float3 right = normalize(cross(up, forward));

    position.xyz += (input.Uv.x - 0.5f) * right * input.Scale;
    position.xyz += (1.0f - input.Uv.y - 0.5f) * up * input.Scale;
    position.w = 1.0f;

    output.Position = WorldPosition(position);
    output.Position = ViewProjection(output.Position);

    float4 view = mul(position, View);
    output.Alpha = saturate(1 - view.z / DrawDistance) * 0.5f;


    output.Uv = input.Uv;

    return output;
}

float4 PS(VertexOutput input) : SV_Target
{
    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.Uv);
    diffuse.rgb = Color.rgb * input.Alpha * 2.0f;
    diffuse.a = diffuse.a * input.Alpha * 4.5f;

    return diffuse;
}

technique11 T0
{
    P_BS_VP(P0, AlphaBlend, VS, PS)
    P_BS_VP(P1, AlphaBlend_AlphaToCoverageEnable, VS, PS)
    P_BS_VP(P2, AdditiveBlend, VS, PS)
    P_BS_VP(P3, AdditiveBlend_AlphaToCoverageEnable, VS, PS)
}