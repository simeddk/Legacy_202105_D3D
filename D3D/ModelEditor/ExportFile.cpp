#include "stdafx.h"
#include "ExportFile.h"
#include "Converter.h"

void ExportFile::Initialize()
{
	//Tank();
	//Kachujin();
}

void ExportFile::Tank()
{
	Converter* conv = new Converter();
	conv->ReadFile(L"Tank/Tank.fbx");
	conv->ExportMesh(L"Tank/Tank");
	SafeDelete(conv);
}

void ExportFile::Kachujin()
{
	Converter* conv = new Converter();
	conv->ReadFile(L"Kachujin/Mesh.fbx");
	conv->ExportMesh(L"Kachujin/Mesh");
	SafeDelete(conv);
}

