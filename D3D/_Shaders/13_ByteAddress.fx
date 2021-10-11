ByteAddressBuffer Input;
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

    uint inAddress = (input.GroupId.x * 240 + input.GroupIndex) * 4;
    float fromCPP = asfloat(Input.Load(inAddress)) * 100;

    uint outAddress = (input.GroupId.x * 240 + input.GroupIndex) * 11 * 4;

    Output.Store3(outAddress + 0, asuint(input.GroupId));
    Output.Store3(outAddress + 12, asuint(input.GroupThreadID));
    Output.Store3(outAddress + 24, asuint(input.DispatchThreadID));
    Output.Store(outAddress + 36, asuint(input.GroupIndex));
    Output.Store(outAddress + 40, asuint(fromCPP));
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