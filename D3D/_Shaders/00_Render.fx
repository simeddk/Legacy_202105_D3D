//-----------------------------------------------------------------------------
//VS_GENERATE
//-----------------------------------------------------------------------------
#define VS_GENERATE \
output.oPosition = input.Position.xyz; \
\
output.Position = WorldPosition(input.Position); \
output.wPosition = output.Position.xyz; \
\
output.Position = ViewProjection(output.Position); \
output.wvpPosition = output.Position; \
output.wvpPosition_Sub = output.Position;\
\
output.sPosition = WorldPosition(input.Position); \
output.sPosition = mul(output.sPosition, Shadow.View); \
output.sPosition = mul(output.sPosition, Shadow.Projection); \
\
output.Normal = WorldNormal(input.Normal); \
output.Tangent = WorldTangent(input.Tangent); \
output.Uv = input.Uv; \
output.Color = input.Color;

///////////////////////////////////////////////////////////////////////////////
#define VS_DEPTH_GENERATE \
output.Position = WorldPosition(input.Position); \
output.Position = mul(output.Position, Shadow.View); \
output.Position = mul(output.Position, Shadow.Projection); \
\
output.sPosition = output.Position;

//-----------------------------------------------------------------------------
//Mesh(Static - Cube, Cylinder... Etc...)
//-----------------------------------------------------------------------------
struct VertexMesh
{
    float4 Position : Position;
    float2 Uv : Uv;
    float3 Normal : Normal;
    float3 Tangent : Tangent;

    matrix Transform : Inst1_Transform;
    float4 Color : Inst2_Color;
};

void SetMeshWorld(inout matrix world, VertexMesh input)
{
    world = input.Transform;

}

MeshOutput VS_Mesh(VertexMesh input)
{
    MeshOutput output = (MeshOutput)0;
    SetMeshWorld(World, input);

    VS_GENERATE

    return output;
}

MeshOutput VS_Mesh_Projector(VertexMesh input)
{
    MeshOutput output = VS_Mesh(input);
    VS_Projector(output.wvpPosition_Sub, input.Position);

    return output;
}

MeshDepthOutput VS_Mesh_Depth(VertexMesh input)
{
    MeshDepthOutput output = (MeshDepthOutput) 0;
    SetMeshWorld(World, input);

    VS_DEPTH_GENERATE

    return output;
}

//-----------------------------------------------------------------------------
//Model(Skeletal - Tank, Airplane... Etc)
//-----------------------------------------------------------------------------
Texture2DArray TransformMap;

struct VertexModel
{
    float4 Position : Position;
    float2 Uv : Uv;
    float3 Normal : Normal;
    float3 Tangent : Tangent;
    float4 BlendIndices : BlendIndices;
    float4 BlendWeights : BlendWeights;

    matrix Transform : Inst1_Transform;
    float4 Color : Inst2_Color;

    uint InstanceID : SV_InstanceID;
};

#define MAX_MODEL_TRANSFORMS 250
cbuffer CB_Bones
{
    Matrix BoneTransforms[MAX_MODEL_TRANSFORMS];

    uint BoneIndex;
};

void SetModelWorld(inout matrix world, VertexModel input)
{
    float4 m0 = TransformMap.Load(int4(BoneIndex * 4 + 0, input.InstanceID, 0, 0));
    float4 m1 = TransformMap.Load(int4(BoneIndex * 4 + 1, input.InstanceID, 0, 0));
    float4 m2 = TransformMap.Load(int4(BoneIndex * 4 + 2, input.InstanceID, 0, 0));
    float4 m3 = TransformMap.Load(int4(BoneIndex * 4 + 3, input.InstanceID, 0, 0));

    matrix transform = matrix(m0, m1, m2, m3);
    world = mul(transform, input.Transform);

}

MeshOutput VS_Model(VertexModel input)
{
    MeshOutput output = (MeshOutput) 0;

    SetModelWorld(World, input);
    
    VS_GENERATE

    return output;
}

MeshOutput VS_Model_Projector(VertexModel input)
{
    MeshOutput output = VS_Model(input);
    VS_Projector(output.wvpPosition_Sub, input.Position);

    return output;
}

MeshDepthOutput VS_Model_Depth(VertexModel input)
{
    MeshDepthOutput output = (MeshDepthOutput) 0;
    SetModelWorld(World, input);

    VS_DEPTH_GENERATE

    return output;
}

//-----------------------------------------------------------------------------
//Animation(Skeletal - Kachujin)
//-----------------------------------------------------------------------------
#define MAX_MODEL_KEYFRAMES 500
#define MAX_MODEL_INSTANCE 500

struct AnimationFrame
{
    int Clip;

    uint CurrFrame;
    uint NextFrame;

    float Time;
    float RunningTime;

    float Speed;

    float2 Padding;
};

struct TweenFrame
{
    float TakeTime;
    float TweenTime;
    float RunningTime;
    float Padding;

    AnimationFrame Curr;
    AnimationFrame Next;
};

cbuffer CB_AnimationFrames
{
    TweenFrame TweenFrames[MAX_MODEL_INSTANCE];
};


void SetAnimationWorld(inout matrix world, VertexModel input)
{
    float indices[4] = { input.BlendIndices.x, input.BlendIndices.y, input.BlendIndices.z, input.BlendIndices.w };
    float weights[4] = { input.BlendWeights.x, input.BlendWeights.y, input.BlendWeights.z, input.BlendWeights.w };

    int clip[2];
    int currFrame[2], nextFrame[2];
    float time[2];

    clip[0] = TweenFrames[input.InstanceID].Curr.Clip;
    currFrame[0] = TweenFrames[input.InstanceID].Curr.CurrFrame;
    nextFrame[0] = TweenFrames[input.InstanceID].Curr.NextFrame;
    time[0] = TweenFrames[input.InstanceID].Curr.Time;

    clip[1] = TweenFrames[input.InstanceID].Next.Clip;
    currFrame[1] = TweenFrames[input.InstanceID].Next.CurrFrame;
    nextFrame[1] = TweenFrames[input.InstanceID].Next.NextFrame;
    time[1] = TweenFrames[input.InstanceID].Next.Time;

    float4 c0, c1, c2, c3;
    float4 n0, n1, n2, n3;

    matrix transform = 0;
    matrix curr = 0, next = 0;
    matrix currAnim = 0, nextAnim = 0;

    [unroll(4)]
    for (int i = 0; i < 4; i++)
    {
        c0 = TransformMap.Load(int4(indices[i] * 4 + 0, currFrame[0], clip[0], 0));
        c1 = TransformMap.Load(int4(indices[i] * 4 + 1, currFrame[0], clip[0], 0));
        c2 = TransformMap.Load(int4(indices[i] * 4 + 2, currFrame[0], clip[0], 0));
        c3 = TransformMap.Load(int4(indices[i] * 4 + 3, currFrame[0], clip[0], 0));
        curr = matrix(c0, c1, c2, c3);

        n0 = TransformMap.Load(int4(indices[i] * 4 + 0, nextFrame[0], clip[0], 0));
        n1 = TransformMap.Load(int4(indices[i] * 4 + 1, nextFrame[0], clip[0], 0));
        n2 = TransformMap.Load(int4(indices[i] * 4 + 2, nextFrame[0], clip[0], 0));
        n3 = TransformMap.Load(int4(indices[i] * 4 + 3, nextFrame[0], clip[0], 0));
        next = matrix(n0, n1, n2, n3);

        currAnim = lerp(curr, next, time[0]);

        [flatten]
        if (clip[1] > -1)
        {
            c0 = TransformMap.Load(int4(indices[i] * 4 + 0, currFrame[1], clip[1], 0));
            c1 = TransformMap.Load(int4(indices[i] * 4 + 1, currFrame[1], clip[1], 0));
            c2 = TransformMap.Load(int4(indices[i] * 4 + 2, currFrame[1], clip[1], 0));
            c3 = TransformMap.Load(int4(indices[i] * 4 + 3, currFrame[1], clip[1], 0));
            curr = matrix(c0, c1, c2, c3);

            n0 = TransformMap.Load(int4(indices[i] * 4 + 0, nextFrame[1], clip[1], 0));
            n1 = TransformMap.Load(int4(indices[i] * 4 + 1, nextFrame[1], clip[1], 0));
            n2 = TransformMap.Load(int4(indices[i] * 4 + 2, nextFrame[1], clip[1], 0));
            n3 = TransformMap.Load(int4(indices[i] * 4 + 3, nextFrame[1], clip[1], 0));
            next = matrix(n0, n1, n2, n3);

            nextAnim = lerp(curr, next, time[1]);

            currAnim = lerp(currAnim, nextAnim, TweenFrames[input.InstanceID].TweenTime);
        }

        transform += mul(weights[i], currAnim);
    }

    world = mul(transform, world);
    //(weight) * (inv * animation * parent) * (World)
}

struct BlendFrame
{
    uint Mode;
    float Alpha;
    float2 Padding;

    AnimationFrame Clips[3];
};

cbuffer CB_BlendFrames
{
    BlendFrame BlendFrames[MAX_MODEL_INSTANCE];
}

void SetBlendWorld(inout matrix world, VertexModel input)
{
    float indices[4] = { input.BlendIndices.x, input.BlendIndices.y, input.BlendIndices.z, input.BlendIndices.w };
    float weights[4] = { input.BlendWeights.x, input.BlendWeights.y, input.BlendWeights.z, input.BlendWeights.w };

    float4 c0, c1, c2, c3;
    float4 n0, n1, n2, n3;

    matrix transform = 0;
    matrix curr = 0, next = 0;
    matrix anim = 0;
    matrix currAnim[3];


    [unroll(4)]
    for (int i = 0; i < 4; i++)
    {
        [unroll(3)]
        for (int k = 0; k < 3; k++)
        {
            c0 = TransformMap.Load(int4(indices[i] * 4 + 0, BlendFrames[input.InstanceID].Clips[k].CurrFrame, BlendFrames[input.InstanceID].Clips[k].Clip, 0));
            c1 = TransformMap.Load(int4(indices[i] * 4 + 1, BlendFrames[input.InstanceID].Clips[k].CurrFrame, BlendFrames[input.InstanceID].Clips[k].Clip, 0));
            c2 = TransformMap.Load(int4(indices[i] * 4 + 2, BlendFrames[input.InstanceID].Clips[k].CurrFrame, BlendFrames[input.InstanceID].Clips[k].Clip, 0));
            c3 = TransformMap.Load(int4(indices[i] * 4 + 3, BlendFrames[input.InstanceID].Clips[k].CurrFrame, BlendFrames[input.InstanceID].Clips[k].Clip, 0));
            curr = matrix(c0, c1, c2, c3);

            n0 = TransformMap.Load(int4(indices[i] * 4 + 0, BlendFrames[input.InstanceID].Clips[k].NextFrame, BlendFrames[input.InstanceID].Clips[k].Clip, 0));;
            n1 = TransformMap.Load(int4(indices[i] * 4 + 1, BlendFrames[input.InstanceID].Clips[k].NextFrame, BlendFrames[input.InstanceID].Clips[k].Clip, 0));;
            n2 = TransformMap.Load(int4(indices[i] * 4 + 2, BlendFrames[input.InstanceID].Clips[k].NextFrame, BlendFrames[input.InstanceID].Clips[k].Clip, 0));;
            n3 = TransformMap.Load(int4(indices[i] * 4 + 3, BlendFrames[input.InstanceID].Clips[k].NextFrame, BlendFrames[input.InstanceID].Clips[k].Clip, 0));;
            next = matrix(n0, n1, n2, n3);

            currAnim[k] = lerp(curr, next, BlendFrames[input.InstanceID].Clips[k].Time);
        }

        float alpha = BlendFrames[input.InstanceID].Alpha;
        int clipIndex[2] = { 0, 1 };
        
        if (alpha > 1)
        {
            clipIndex[0] = 1;
            clipIndex[1] = 2;

            alpha -= 1.0f;
        }

        anim = lerp(currAnim[clipIndex[0]], currAnim[clipIndex[1]], alpha);

        transform += mul(weights[i], anim);
    }

    world = mul(transform, world);
}

MeshOutput VS_Animation(VertexModel input)
{
    MeshOutput output = (MeshOutput) 0;

    World = input.Transform;

    if (BlendFrames[input.InstanceID].Mode == 0)
        SetAnimationWorld(World, input);
    else
        SetBlendWorld(World, input);

    VS_GENERATE

    return output;
}

MeshOutput VS_Animation_Projector(VertexModel input)
{
    MeshOutput output = VS_Animation(input);
    VS_Projector(output.wvpPosition_Sub, input.Position);

    return output;
}

MeshDepthOutput VS_Animation_Depth(VertexModel input)
{
    MeshDepthOutput output = (MeshDepthOutput) 0;

    World = input.Transform;

    if (BlendFrames[input.InstanceID].Mode == 0)
        SetAnimationWorld(World, input);
    else
        SetBlendWorld(World, input);

    VS_DEPTH_GENERATE

    return output;
}

//-----------------------------------------------------------------------------
//DynamicCubeMap - GS
//-----------------------------------------------------------------------------
struct DynamicCubeDesc
{
    uint Type;
    float Alpha;
    float RefractAmount;
    float Padding;

    float FresnelAmount;
    float FresnelBias;
    float FresnelScale;
    float Padding2;

    matrix Views[6];
    matrix Projection;
};

cbuffer CB_DynamicCube
{
    DynamicCubeDesc DynamicCube;
};

[maxvertexcount(18)]
void GS_DynamicCube_PreRender(triangle MeshOutput input[3], inout TriangleStream<MeshOutput> stream)
{
    int vertex = 0;
    MeshOutput output = (MeshOutput)0;

    [unroll(6)]
    for (int i = 0; i < 6; i++)
    {
        output.TargetIndex = i;

        [unroll(3)]
        for (vertex = 0; vertex < 3; vertex++)
        {
            output.Position = mul(float4(input[vertex].wPosition, 1), DynamicCube.Views[i]);
            output.Position = mul(output.Position, DynamicCube.Projection);

            output.oPosition = input[vertex].oPosition;
            output.wPosition = input[vertex].wPosition;
            output.Normal = input[vertex].Normal;
            output.Tangent = input[vertex].Tangent;
            output.Uv = input[vertex].Uv;
            output.Color = input[vertex].Color;

            stream.Append(output);
        }

        stream.RestartStrip();

    }
}

//-----------------------------------------------------------------------------
//DynamicCubeMap - PS
//-----------------------------------------------------------------------------
float4 PS_DynamicCube(MeshOutput input) : SV_Target
{
    float4 diffuse = 0;

    float3 view = normalize(input.wPosition - ViewPosition());
    float3 normal = normalize(input.Normal);

    float3 reflection = reflect(view, normal);
    float3 refraction = refract(view, normal, DynamicCube.RefractAmount);

    //ÀÏ¹Ý
    if (DynamicCube.Type == 0)
    {
        diffuse = DynamicCubeMap.Sample(LinearSampler, input.oPosition);
        diffuse.a = 1.0f;
    }
    //¹Ý»ç
    else if (DynamicCube.Type == 1)
    {
        diffuse = DynamicCubeMap.Sample(LinearSampler, reflection);
        diffuse.a = DynamicCube.Alpha;
    }
    //±¼Àý
    else if (DynamicCube.Type == 2)
    {
        diffuse = DynamicCubeMap.Sample(LinearSampler, -refraction);
        diffuse.a = DynamicCube.Alpha;
    }
    //ÇÁ¸®³Ú
    else if (DynamicCube.Type == 3)
    {
        float4 phong = PS_Phong(input);
        diffuse = DynamicCubeMap.Sample(LinearSampler, reflection);

        float4 fresnel = 
        DynamicCube.FresnelBias + (1.0f - DynamicCube.FresnelScale)
        * pow(abs(1.0f - dot(view, normal)), DynamicCube.FresnelAmount);

        diffuse = DynamicCube.FresnelAmount * diffuse + lerp(phong, diffuse, fresnel);
        diffuse *= 0.75f;

        diffuse.a = DynamicCube.Alpha;
    }

    return diffuse;
}

float4 PS_Sky(MeshOutput input) : SV_Target
{
    return SkyCubeMap.Sample(LinearSampler, input.oPosition);
}