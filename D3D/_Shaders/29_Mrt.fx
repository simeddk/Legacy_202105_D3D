#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"

//-----------------------------------------------------------------------------
//PreRender
//-----------------------------------------------------------------------------
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

float2 PixelSize;
float4 PS_PreRender_Interace(VertexOutput input) : SV_Target
{
    float Strength = 0.5f;
    int InteraceValue = 5;

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

//-----------------------------------------------------------------------------
//Mrt
//-----------------------------------------------------------------------------
struct PixelOutput
{
    float4 Color0 : SV_Target0;
    float4 Color1 : SV_Target1;
    float4 Color2 : SV_Target2;
};

PixelOutput PS_Mrt(VertexOutput input)
{
    PixelOutput output;

    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.Uv);

    output.Color0 = diffuse;
    output.Color1 = float4(1 - diffuse.rgb, 1);
    output.Color2 = float4(diffuse.rgb + float3(1, 0, 0), 1);

    return output;
}

technique11 T0
{
    P_VP(P0, VS, PS_PreRender_Interace)
    P_VP(P1, VS, PS_Mrt)
}