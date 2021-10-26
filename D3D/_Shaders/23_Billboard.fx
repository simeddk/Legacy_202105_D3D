#include "00_Global.fx"
#include "00_Light.fx"

struct VertexInput
{
    float4 Position : Position;
    float2 Uv : Uv;
    float2 Scale : Scale;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float2 Uv : Uv;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    float4 position = input.Position;
    
    float3 up = float3(0, 1, 0);
    float3 forward = normalize(position.xyz - ViewPosition());
    float3 right = normalize(cross(up, forward));

    position.xyz += (input.Uv.x - 0.5f) * right * input.Scale.x;
    position.xyz += (1.0f - input.Uv.y - 0.5f) * up * input.Scale.y;
    position.w = 1.0f;

    output.Position = WorldPosition(position);
    output.Position = ViewProjection(output.Position);

    output.Uv = input.Uv;

    return output;
}

float4 PS(VertexOutput input) : SV_Target
{
    float4 diffuse =  DiffuseMap.Sample(LinearSampler, input.Uv);
  
    return diffuse;
}

technique11 T0
{
    P_BS_VP(P0, AlphaBlend, VS, PS)
}