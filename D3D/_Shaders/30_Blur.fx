#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"

struct VertexOutput
{
    float4 Position : SV_Position;
    float2 Uv : UV;
};

VertexOutput VS(float4 position : Position)
{
    VertexOutput output;

    output.Position = position;
    output.Uv.x = position.x * 0.5f + 0.5f;
    output.Uv.y = -position.y * 0.5f + 0.5f;

    return output;
}

float4 PS_Diffuse(VertexOutput input) : SV_Target
{
    return DiffuseMap.Sample(LinearSampler, input.Uv);
}

float2 PixelSize;
uint BlurCount = 8;
float4 PS_Blur(VertexOutput input) : SV_Target
{
    float2 offset[9] =
    {
        float2(-1, -1), float2(+0, -1), float2(+1, -1),
        float2(-1, +0), float2(+0, +0), float2(+1, +0),
        float2(-1, +1), float2(+0, +1), float2(+1, +1)
    };

    float3 sum = 0;
    for (uint blur = 0; blur < BlurCount; blur++)
    {
        for (uint i = 0; i < 9; i++)
        {
            float u = offset[i].x * PixelSize.x * (float) blur;
            float v = offset[i].y * PixelSize.y * (float) blur;

            float2 uv = input.Uv + float2(u, v);
            sum += DiffuseMap.Sample(LinearSampler, uv).rgb;
        }
    }

    sum /= BlurCount * 9;

    return float4(sum, 1);
}

uint RadialBlurCount = 8;
float RadialBlurRadius = 0.0f;
float RadialBlurAmout = 0.04f;
float2 RadialCenter = float2(0.5f, 0.5f);
float4 PS_RadialBlur(VertexOutput input) : SV_Target
{
    float2 centerDirection = input.Uv - RadialCenter;
    float2 radius = length(centerDirection);
    centerDirection /= radius;

    radius = radius * 2 / RadialBlurRadius;
    radius = saturate(radius);

    float2 uv = centerDirection * radius * radius * RadialBlurAmout / RadialBlurCount;

    float4 sum = 0;
    for (uint i = 0; i < RadialBlurCount; i++)
    {
        sum += DiffuseMap.Sample(LinearSampler, input.Uv);
        input.Uv -= uv;//
    }

    sum /= RadialBlurCount;

    return float4(sum.rgb, 1);
}

technique11 T0
{
    P_VP(P0, VS, PS_Diffuse)
    P_VP(P1, VS, PS_Blur)
    P_VP(P2, VS, PS_RadialBlur)
}