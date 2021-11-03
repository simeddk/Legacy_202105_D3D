#include "00_Global.fx"

Texture2D ParticleMap;

struct ParticleDesc
{
    float4 MinColor;
    float4 MaxColor;

    float3 Gravity;
    float EndVelocity;

    float2 StartSize;
    float2 EndSize;

    float2 RotateSpeed;
    float ReadyTime;
    float ReadyRandomTime;

    float CurrentTime;
};

cbuffer CB_Particle
{
    ParticleDesc Particle;
};

struct VertexInput
{
    float4 Position : Position;
    float2 Corner : Corner;
    float3 Velocity : Velocity;
    float4 Random : Random; //x:PlayRate, y:Size, z:Rotate, w:Color
    float Time : Time;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float4 Color : Color;
    float2 Uv : Uv;
};

float4 ComputePosition(float3 position, float3 velocity, float age, float normalizedAge)
{
    float start = length(velocity);
    float end = start * Particle.EndVelocity;

    float integral = start * normalizedAge + (end - start) * normalizedAge / 2;

    position += normalize(velocity) * integral * Particle.ReadyTime;
    position += Particle.Gravity * normalizedAge * age;

    return ViewProjection(float4(position, 1));
}

float ComputeSize(float value, float normalizeAge)
{
    float start = lerp(Particle.StartSize.x, Particle.StartSize.y, value);
    float end = lerp(Particle.EndSize.x, Particle.EndSize.y, value);

    return lerp(start, end, normalizeAge);
}

float2x2 ComputeRotation(float value, float age)
{
    float speed = lerp(Particle.RotateSpeed.x, Particle.RotateSpeed.y, value);
    float radian = speed * age;

    float c = cos(radian);
    float s = sin(radian);
    
    return float2x2(c, -s, s, c);
}

float4 ComputeColor(float value, float normalizedAge)
{
    float4 color = lerp(Particle.MinColor, Particle.MaxColor, value);
    color.a *= normalizedAge * (1 - normalizedAge) * (1 - normalizedAge) * 6.7f;

    return color;
}

VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    float age = Particle.CurrentTime - input.Time;
    age *= input.Random.x * Particle.ReadyRandomTime + 1;

    float normalizeAge = saturate(age / Particle.ReadyTime);

    output.Position = ComputePosition(input.Position.xyz, input.Velocity, age, normalizeAge);

    float size = ComputeSize(input.Random.y, normalizeAge);
    float2x2 rotation = ComputeRotation(input.Random.z, age);

    output.Position.xy += mul(input.Corner, rotation) * size * 0.5f;

    output.Uv = (input.Corner + 1.0f) * 0.5f;
    output.Color = ComputeColor(input.Random.w, normalizeAge);

    return output;
}

float4 PS(VertexOutput input) : SV_Target
{
    return ParticleMap.Sample(LinearSampler, input.Uv) * input.Color;
}

technique11 T0
{
    //P_VP(P0, VS, PS)
    //P_BS_VP(P1, AdditiveBlend, VS, PS)
    //P_BS_VP(P2, AlphaBlend, VS, PS)

    //P_BS_VP(P3, AdditiveBlend_AlphaToCoverageEnable, VS, PS)
    //P_BS_VP(P4, AlphaBlend_AlphaToCoverageEnable, VS, PS)

    //P_DSS_VP(P5, DepthEnable_False, VS, PS)
    //P_DSS_BS_VP(P6, DepthEnable_False, AdditiveBlend, VS, PS)
    //P_DSS_BS_VP(P7, DepthEnable_False, AlphaBlend, VS, PS)

    P_DSS_VP(P0, PixelDepthEnable_False, VS, PS)
    P_DSS_BS_VP(P1, PixelDepthEnable_False, AdditiveBlend, VS, PS)
    P_DSS_BS_VP(P2, PixelDepthEnable_False, AlphaBlend, VS, PS)
}