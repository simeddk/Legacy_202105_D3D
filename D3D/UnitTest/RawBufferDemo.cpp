#include "stdafx.h"
#include "RawBufferDemo.h"

void RawBufferDemo::Initialize()
{
	Shader* shader = new Shader(L"13_ByteAddress.fxo");

	UINT size = 10 * 8 * 3 * 2;

	struct Output
	{
		UINT GroupId[3];
		UINT GroupThreadID[3];
		UINT DispatchThreadID[3];
		UINT GroupIndex;
	};

	RawBuffer* rawBuffer = new RawBuffer(nullptr, 0, sizeof(Output) * size);

	shader->AsUAV("Output")->SetUnorderedAccessView(rawBuffer->UAV());
	shader->Dispatch(0, 0, 2, 1, 1);

	Output* output = new Output[size];
	rawBuffer->CopyFromOutput(output);

	FILE* fp;
	fopen_s(&fp, "../RawBuffer.csv", "w");

	for (UINT i = 0; i < size; i++)
	{
		Output temp = output[i];

		fprintf
		(
			fp,
			"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n",
			temp.GroupId[0], temp.GroupId[1], temp.GroupId[2],
			temp.GroupThreadID[0], temp.GroupThreadID[1], temp.GroupThreadID[2],
			temp.DispatchThreadID[0], temp.DispatchThreadID[1], temp.DispatchThreadID[2],
			temp.GroupIndex
		);
	}

	fclose(fp);
}
