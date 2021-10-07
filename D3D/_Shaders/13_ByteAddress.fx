RWByteAddressBuffer Output;

struct Group
{
    uint3 GroupId;
    uint3 GroupThreadID;
    uint3 DispatchThreadID;
    uint GroupIndex;
};

struct ComputeInput
{
    uint3 GroupId : SV_GroupId;
    uint3 GroupThreadID : SV_GroupThreadID;
    uint3 DispatchThreadID : SV_DispatchThreadID;
    uint GroupIndex : SV_GroupIndex;
};

[numthreads(10, 8, 3)]
void CS(ComputeInput input)
{
    Group group;
    group.GroupId = asuint(input.GroupId);
    group.GroupThreadID = asuint(input.GroupThreadID);
    group.DispatchThreadID = asuint(input.DispatchThreadID);
    group.GroupIndex = asuint(input.GroupIndex);

    uint OutAddress = (input.GroupId.x * 240 + input.GroupIndex) * 10 * 4;

    Output.Store3(OutAddress + 0, asuint(input.GroupId));
    Output.Store3(OutAddress + 12, asuint(input.GroupThreadID));
    Output.Store3(OutAddress + 24, asuint(input.DispatchThreadID));
    Output.Store(OutAddress + 36, asuint(input.GroupIndex));
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