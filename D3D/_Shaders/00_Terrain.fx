Texture2D BaseMap;

struct VertexTerrain
{
    float4 Position : Position;
    float2 Uv : Uv;
    float3 Normal : Normal;
    float3 Tangent : Tangent;
    float4 Color : Color;
};


//-----------------------------------------------------------------------------
//Shader
//-----------------------------------------------------------------------------
MeshOutput VS_Terrain(VertexTerrain input)
{
    MeshOutput output = (MeshOutput) 0;

    VS_GENERATE

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
    return BaseMap.Sample(LinearSampler, input.Uv);
}