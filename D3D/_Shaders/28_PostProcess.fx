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

float Power = 2.0f;
float2 Scale = float2(2, 2);
float4 PS_Vignette(VertexOutput input) : SV_Target
{
    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.Uv);

    float radius = length((input.Uv - 0.5f) * 2 / Scale);
    float vignette = pow(abs(radius + 1e-6f), Power);

    return saturate(1 - vignette) * diffuse;
}

float Strength = 1.0f;
int InteraceValue = 2;
float4 PS_Interace(VertexOutput input) : SV_Target
{
    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.Uv);

    float vpHeight = 1.0f / PixelSize.y;
    int value = (int) ((floor(input.Uv.y * vpHeight) % InteraceValue) / (InteraceValue / 2));

    [flatten]
    if (value)
    {
        float3 grayScale = float3(0.2126f, 0.7152f, 0.0722f);
        float gray = dot(grayScale, diffuse.rgb);

        gray = min(0.999f, gray);

        diffuse.rgb = lerp(diffuse.rgb, diffuse.rgb * gray, Strength);
    }

    return diffuse;
}

float LensPower = 1.0f;
float3 Distortion = -0.02f;
float4 PS_Distortion(VertexOutput input) : SV_Target
{
    float2 uv = input.Uv * 2 - 1;

    float2 vpSize = float2(1.0f / PixelSize.x, 1.0f / PixelSize.y);
    float aspect = vpSize.x / vpSize.y;

    float radiusSqaured = aspect * aspect + uv.x * uv.x + uv.y * uv.y;
    float radius = sqrt(radiusSqaured);

    float3 d = Distortion * pow(abs(radius + 1e-6f), LensPower) + 1.0f;

    float2 r = (d.r * uv + 1) * 0.5f;
    float2 g = (d.g * uv + 1) * 0.5f;
    float2 b = (d.b * uv + 1) * 0.5f;

    float4 color = 0;
    color.r = DiffuseMap.Sample(LinearSampler, r).r;
    color.g = DiffuseMap.Sample(LinearSampler, g).g;
    color.b = DiffuseMap.Sample(LinearSampler, b).b;

    return color;

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
    P_VP(P8, VS, PS_Vignette)
    P_VP(P9, VS, PS_Interace)
    P_VP(P10, VS, PS_Distortion)
}