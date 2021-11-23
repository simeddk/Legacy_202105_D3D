Texture2D BaseMap;
Texture2D Layer1AlphaMap;
Texture2D Layer1ColorMap;

Texture2D HeightMap;

struct VertexTerrain
{
    float4 Position : Position;
    float2 Uv : Uv;
    float3 Normal : Normal;
    float3 Tangent : Tangent;
    float4 Color : Color;
};

//-----------------------------------------------------------------------------
//Brush
//-----------------------------------------------------------------------------
struct BrushDesc
{
    float4 Color;
    float3 Location;
    uint Type;
    uint Range;
};

cbuffer CB_TerrainBrush
{
    BrushDesc TerrainBrush;
};

float4 GetBrushColor(float3 wPosition)
{
    [flatten]
    if (TerrainBrush.Type == 1)
    {
        [flatten]
        if ((wPosition.x >= (TerrainBrush.Location.x - TerrainBrush.Range)) &&
            (wPosition.x <= (TerrainBrush.Location.x + TerrainBrush.Range)) &&
            (wPosition.z >= (TerrainBrush.Location.z - TerrainBrush.Range)) &&
            (wPosition.z <= (TerrainBrush.Location.z + TerrainBrush.Range)))
        {
            return TerrainBrush.Color;
        }
    }

    [flatten]
    if (TerrainBrush.Type == 2)
    {
        float dx = wPosition.x - TerrainBrush.Location.x;
        float dz = wPosition.z - TerrainBrush.Location.z;
        float dist = sqrt(dx * dx + dz * dz);

        [flatten]
        if (dist <= TerrainBrush.Range)
            return TerrainBrush.Color;
    }

    return float4(0, 0, 0, 0);

}

//-----------------------------------------------------------------------------
//Line
//-----------------------------------------------------------------------------
struct LineDesc
{
    float4 Color;
    uint Visible;
    float Thickness;
    float Size;
};

cbuffer CB_TerrainLine
{
    LineDesc TerrainLine;
};

float4 GetLineColor(float3 wPosition)
{
    [flatten]
    if (TerrainLine.Visible < 1)
        return float4(0, 0, 0, 0);

    float2 grid = wPosition.xz / TerrainLine.Size;
    float2 range = abs(frac(grid - 0.5f) - 0.5f);
    float2 weight = fwidth(grid);

    float2 pixel = range / weight;

    float thick = saturate(min(pixel.r, pixel.g) - TerrainLine.Thickness);

    return lerp(TerrainLine.Color, float4(0, 0, 0, 0), thick);
}


//-----------------------------------------------------------------------------
//Shader
//-----------------------------------------------------------------------------
MeshOutput VS_Terrain(VertexTerrain input)
{
    MeshOutput output = (MeshOutput) 0;

    VS_GENERATE

    //output.Color = GetBrushColor(output.wPosition);

    return output;
}

MeshDepthOutput VS_Terrain_Depth(VertexTerrain input)
{
    MeshDepthOutput output;

    VS_DEPTH_GENERATE

    return output;
}

MeshOutput VS_Terrain_Projector(VertexTerrain input)
{
    MeshOutput output = VS_Terrain(input);
    VS_Projector(output.wvpPosition_Sub, input.Position);

    return output;
}


float4 PS_Terrain(MeshOutput input) : SV_Target
{
    float4 color = BaseMap.Sample(LinearSampler, input.Uv);

    float alphaMap = Layer1AlphaMap.Sample(LinearSampler, input.Uv).r;
    float4 colorMap = Layer1ColorMap.Sample(LinearSampler, input.Uv);

    if (alphaMap > 0.0f)
        color = lerp(color, colorMap, alphaMap);

    Material.Diffuse = color;
    color = CalcualteFogColor(PS_Shadow(input.sPosition, PS_Phong(input)), input.wPosition);

   color += GetBrushColor(input.wPosition);
   color += GetLineColor(input.wPosition);

    return color;
}