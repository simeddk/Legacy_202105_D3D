#include "stdafx.h"
#include "ExportFile.h"
#include "Converter.h"

void ExportFile::Initialize()
{
	Tank();
}

void ExportFile::Tank()
{
	Converter* conv = new Converter();
	conv->ReadFile(L"Tank/Tank.fbx");
	conv->ExportMesh(L"Tank/Tank");
	SafeDelete(conv);
}

