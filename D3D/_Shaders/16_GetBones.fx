#include "00_Global.fx"

struct BoneDesc
{
    matrix Transform;
};
StructuredBuffer<BoneDesc> InputBones;
RWStructuredBuffer<BoneDesc> OutputBones;

#define MAX_MODEL_TRANSFORMS 250

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
    TweenFrame TweenFrames;
};

Texture2DArray TransformMap;

void SetAnimationWorld(inout matrix world, uint3 id)
{
    int clip[2];
    int currFrame[2], nextFrame[2];
    float time[2];

    clip[0] = TweenFrames.Curr.Clip;
    currFrame[0] = TweenFrames.Curr.CurrFrame;
    nextFrame[0] = TweenFrames.Curr.NextFrame;
    time[0] = TweenFrames.Curr.Time;

    clip[1] = TweenFrames.Next.Clip;
    currFrame[1] = TweenFrames.Next.CurrFrame;
    nextFrame[1] = TweenFrames.Next.NextFrame;
    time[1] = TweenFrames.Next.Time;

    float4 c0, c1, c2, c3;
    float4 n0, n1, n2, n3;

    //matrix transform = 0;
    matrix curr = 0, next = 0;
    matrix currAnim = 0, nextAnim = 0;

   
    c0 = TransformMap.Load(int4(id.x * 4 + 0, currFrame[0], clip[0], 0));
    c1 = TransformMap.Load(int4(id.x * 4 + 1, currFrame[0], clip[0], 0));
    c2 = TransformMap.Load(int4(id.x * 4 + 2, currFrame[0], clip[0], 0));
    c3 = TransformMap.Load(int4(id.x * 4 + 3, currFrame[0], clip[0], 0));
    curr = matrix(c0, c1, c2, c3);

    n0 = TransformMap.Load(int4(id.x * 4 + 0, nextFrame[0], clip[0], 0));
    n1 = TransformMap.Load(int4(id.x * 4 + 1, nextFrame[0], clip[0], 0));
    n2 = TransformMap.Load(int4(id.x * 4 + 2, nextFrame[0], clip[0], 0));
    n3 = TransformMap.Load(int4(id.x * 4 + 3, nextFrame[0], clip[0], 0));
    next = matrix(n0, n1, n2, n3);

    currAnim = lerp(curr, next, time[0]);

    [flatten]
    if (clip[1] > -1)
    {
        c0 = TransformMap.Load(int4(id.x * 4 + 0, currFrame[1], clip[1], 0));
        c1 = TransformMap.Load(int4(id.x * 4 + 1, currFrame[1], clip[1], 0));
        c2 = TransformMap.Load(int4(id.x * 4 + 2, currFrame[1], clip[1], 0));
        c3 = TransformMap.Load(int4(id.x * 4 + 3, currFrame[1], clip[1], 0));
        curr = matrix(c0, c1, c2, c3);

        n0 = TransformMap.Load(int4(id.x * 4 + 0, nextFrame[1], clip[1], 0));
        n1 = TransformMap.Load(int4(id.x * 4 + 1, nextFrame[1], clip[1], 0));
        n2 = TransformMap.Load(int4(id.x * 4 + 2, nextFrame[1], clip[1], 0));
        n3 = TransformMap.Load(int4(id.x * 4 + 3, nextFrame[1], clip[1], 0));
        next = matrix(n0, n1, n2, n3);

        nextAnim = lerp(curr, next, time[1]);

        currAnim = lerp(currAnim, nextAnim, TweenFrames.TweenTime);
    }
   
    world = mul(currAnim, world);
}

struct BlendFrame
{
    uint Mode;
    float Alpha;
    float2 Padding;

    AnimationFrame Clips[3];
};

cbuffer CB_BlendingFrames
{
    BlendFrame BlendFrames;
}

void SetBlendWorld(inout matrix world, uint3 id)
{
    float4 c0, c1, c2, c3;
    float4 n0, n1, n2, n3;

    matrix transform = 0;
    matrix curr = 0, next = 0;
    matrix anim = 0;
    matrix currAnim[3];

    
    [unroll(3)]
    for (int k = 0; k < 3; k++)
    {
        c0 = TransformMap.Load(int4(id.x * 4 + 0, BlendFrames.Clips[k].CurrFrame, BlendFrames.Clips[k].Clip, 0));
        c1 = TransformMap.Load(int4(id.x * 4 + 1, BlendFrames.Clips[k].CurrFrame, BlendFrames.Clips[k].Clip, 0));
        c2 = TransformMap.Load(int4(id.x * 4 + 2, BlendFrames.Clips[k].CurrFrame, BlendFrames.Clips[k].Clip, 0));
        c3 = TransformMap.Load(int4(id.x * 4 + 3, BlendFrames.Clips[k].CurrFrame, BlendFrames.Clips[k].Clip, 0));
        curr = matrix(c0, c1, c2, c3);

        n0 = TransformMap.Load(int4(id.x * 4 + 0, BlendFrames.Clips[k].NextFrame, BlendFrames.Clips[k].Clip, 0));;
        n1 = TransformMap.Load(int4(id.x * 4 + 1, BlendFrames.Clips[k].NextFrame, BlendFrames.Clips[k].Clip, 0));;
        n2 = TransformMap.Load(int4(id.x * 4 + 2, BlendFrames.Clips[k].NextFrame, BlendFrames.Clips[k].Clip, 0));;
        n3 = TransformMap.Load(int4(id.x * 4 + 3, BlendFrames.Clips[k].NextFrame, BlendFrames.Clips[k].Clip, 0));;
        next = matrix(n0, n1, n2, n3);

        currAnim[k] = lerp(curr, next, BlendFrames.Clips[k].Time);
    }

    float alpha = BlendFrames.Alpha;
    int clipIndex[2] = { 0, 1 };
        
    if (alpha > 1)
    {
        clipIndex[0] = 1;
        clipIndex[1] = 2;

        alpha -= 1.0f;
    }

    anim = lerp(currAnim[clipIndex[0]], currAnim[clipIndex[1]], alpha);

    world = mul(anim, world);
}


[numthreads(MAX_MODEL_TRANSFORMS, 1, 1)]
void CS(uint3 id : SV_GroupThreadID)
{
    matrix world = World;

    if (BlendFrames.Mode == 0)
        SetAnimationWorld(world, id);
    else
        SetBlendWorld(world, id);

    world = mul(InputBones[id.x].Transform, world);


    OutputBones[id.x].Transform = world;
}

technique11 T0
{
    pass P0
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);

        SetComputeShader(CompileShader(cs_5_0, CS()));
    }
}