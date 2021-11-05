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

float4 PS_inverse(VertexOutput input) : SV_Target
{
    return 1.0f - DiffuseMap.Sample(LinearSampler, input.Uv);
}

float4 PS_GrayScale(VertexOutput input) : SV_Target
{
    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.Uv);
    return (diffuse.r + diffuse.g + diffuse.b) * 0.333333f;
}

float4 PS_GrayScale2(VertexOutput input) : SV_Target
{
    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.Uv);
    float3 weight = float3(0.2126f, 0.7152f, 0.0722f);
    float gray = dot(diffuse.rgb, weight);
    return float4(gray, gray, gray, 1.0f);
}

float3x3 ColorMatrix = float3x3
(
    0.393f, 0.769f, 0.189f, //R
    0.349f, 0.686f, 0.168f, //G
    0.272f, 0.534f, 0.131f //B
);

float4 PS_Matrix(VertexOutput input) : SV_Target
{
    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.Uv);
    float3 color = mul(diffuse.rgb, transpose(ColorMatrix));
    return float4(color, 1.0f);
}

float Saturation = 0;
float4 PS_Saturation(VertexOutput input) : SV_Target
{
    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.Uv);
    float3 weight = float3(0.2126f, 0.7152f, 0.0722f);
    float gray = dot(diffuse.rgb, weight);

    diffuse.rgb = lerp(gray, diffuse, Saturation);
    diffuse.a = 1.0f;
    return diffuse;
}

float2 PixelSize;
float Sharpness;
float4 PS_Sharpness(VertexOutput input) : SV_Target
{
    float4 center = DiffuseMap.Sample(LinearSampler, input.Uv);
    float4 top = DiffuseMap.Sample(LinearSampler, input.Uv + float2(0, -PixelSize.y));
    float4 bottom = DiffuseMap.Sample(LinearSampler, input.Uv + float2(0, +PixelSize.y));
    float4 left = DiffuseMap.Sample(LinearSampler, input.Uv + float2(-PixelSize.x, 0));
    float4 right = DiffuseMap.Sample(LinearSampler, input.Uv + float2(+PixelSize.x, 0));

    float4 edge = center * 4 - top - bottom - left - right;
    
    float3 weight = float3(0.2126f, 0.7152f, 0.0722f);
    float gray = dot(edge.rgb, weight);

    return center + (Sharpness * float4(gray, gray, gray, 1.0f));

}

float2 WiggleOffset = float2(10, 10);
float2 WiggleAmout = float2(0.01f, 0.01f);
float4 PS_Wiggle(VertexOutput input) : SV_Target
{
    float2 uv = input.Uv;
    uv.x += sin(Time + uv.x * WiggleOffset.x) * WiggleAmout.x;
    uv.y += cos(Time + uv.y * WiggleOffset.y) * WiggleAmout.y;

    return DiffuseMap.Sample(LinearSampler, uv);

}

technique11 T0
{
    P_VP(P0, VS, PS_Diffuse)
    P_VP(P1, VS, PS_inverse)
    P_VP(P2, VS, PS_GrayScale)
    P_VP(P3, VS, PS_GrayScale2)
    P_VP(P4, VS, PS_Matrix)
    P_VP(P5, VS, PS_Saturation)
    P_VP(P6, VS, PS_Sharpness)
    P_VP(P7, VS, PS_Wiggle)
}