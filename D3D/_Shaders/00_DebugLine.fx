#include "00_Global.fx"

struct VertexInput
{
    float4 Position : Position;
    float4 Color : Color;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float4 Color : Color;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;

    output.Position = WorldPosition(input.Position);
    output.Position = ViewProjection(output.Position);

    output.Color = input.Color;

    return output;
}

float4 PS(VertexOutput input) : SV_Target
{
    return input.Color;
}

technique11 T0
{
    P_VP(P0, VS, PS)
}