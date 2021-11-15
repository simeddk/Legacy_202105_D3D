#include "stdafx.h"
#include "Editor.h"
#include "Brush.h"

void Editor::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(20, 0, 0);
	Context::Get()->GetCamera()->Position(126, 35, 39);
	((Freedom*)Context::Get()->GetCamera())->Speed(100.0f, 2.0f);

	heightMapFileDirectory = L"Terrain/";
	heightMapFileName = L"Gray256.png";

	shader = new Shader(L"35_Terrain.fxo");
	shadow = new Shadow(shader, Vector3(128, 0, 128), 65);

	sky = new CubeSky(L"Environment/Mountain1024.dds", shader);
	brush = new Brush(shader, terrain);

	openFunc = bind(&Editor::OpenComplete, this, placeholders::_1);

	wstring path = L"Terrain/Gray256.dds";
	terrain = new Terrain(shader, path);
	terrain->BaseMap(L"Terrain/Dirt3.png");
	terrain->Layer1(L"Terrain/Dirt.png");

	brush = new Brush(shader, terrain);
}

void Editor::Destroy()
{
	SafeDelete(shader);
	SafeDelete(shadow);
	SafeDelete(sky);
	SafeDelete(terrain);
	SafeDelete(brush);
}

void Editor::Update()
{
	ImGui::SliderFloat3("LightDirectoin", Lighting::Get()->Direction(), -1, 1);

	//버튼 - 파일 열기창
	{
		if (ImGui::Button("Open HeightMap"))
		{
			Path::OpenFileDialog(heightMapFileName, Path::ImageFilter, heightMapFileDirectory, openFunc);

			SafeDelete(heightMapFile);
			heightMapFile = new Texture(heightMapFileDirectory + heightMapFileName);
		}
	}

	//콤보박스 - 채널 선택
	const char* items[] = { "Red", "Green", "Blue", "Alpha" };
	ImGui::Combo("Channel", &selectedChannel, items, 4);

	//버튼 - 높이 정보 -> 위에서 선택한 채널에 저장
	if (ImGui::Button("Convert Map File"))
	{
		ToDataMapFile();
	}

	//이미지 뷰어 - 선택된 높이맵
	if (heightMapFile != nullptr)
		ImGui::Image(heightMapFile->SRV(), ImVec2(128, 128));

	//버튼 - dds 선택 시 지형 생성
	ImGui::Separator();
	if (ImGui::CollapsingHeader("Data Map List", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (UINT i = 0; i < dataMapList.size(); i++)
		{
			if (ImGui::Button(String::ToString(dataMapList[i]).c_str(), ImVec2(200, 0)))
			{
				SafeDelete(terrain);
				SafeDelete(brush);

				wstring path = L"Terrain/" + dataMapList[i] + L".dds";
				terrain = new Terrain(shader, path);
				terrain->BaseMap(L"Terrain/Snow.jpg");
				terrain->Layer1(L"Terrain/Dirt.png");

				brush = new Brush(shader, terrain);
			}
		}
	}

	if (terrain != nullptr)
	{
		terrain->Update();
		brush->Update();
	}

	
	sky->Update();
	UpdateDataMapFileList();
}

void Editor::Render()
{
	sky->Pass(4);
	sky->Render();

	if (terrain != nullptr)
	{
		terrain->Pass(8);
		terrain->Render();

		brush->Render();
	}
}

void Editor::OpenComplete(wstring fileName)
{
	heightMapFileName = Path::GetFileName(fileName);
}

void Editor::ToDataMapFile()
{
	if (heightMapFile == nullptr) return;

	//한번 만들어진 텍스쳐(ComInterface)의 desc를 수정할 수 없으므로, 복사 후 desc 수정
	ID3D11Texture2D* srcTexture = heightMapFile->GetTexture();
	D3D11_TEXTURE2D_DESC srcDesc;
	srcTexture->GetDesc(&srcDesc);

	//원본 높이맵(srcTexture) -> readTexture 복사
	ID3D11Texture2D* readTexture;
	D3D11_TEXTURE2D_DESC readDesc;
	ZeroMemory(&readDesc, sizeof(D3D11_TEXTURE2D_DESC));
	readDesc.Width = srcDesc.Width;
	readDesc.Height = srcDesc.Height;
	readDesc.ArraySize = 1;
	readDesc.Format = srcDesc.Format;
	readDesc.MipLevels = 1;
	readDesc.SampleDesc = srcDesc.SampleDesc;
	readDesc.Usage = D3D11_USAGE_STAGING;
	readDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	Check(D3D::GetDevice()->CreateTexture2D(&readDesc, nullptr, &readTexture));
	D3D::GetDC()->CopyResource(readTexture, srcTexture);

	UINT* pixels = new UINT[readDesc.Width * readDesc.Height];
	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(readTexture, 0, D3D11_MAP_READ, 0, &subResource);
	{
		memcpy(pixels, subResource.pData, sizeof(UINT) * readDesc.Width * readDesc.Height);
	}
	D3D::GetDC()->Unmap(readTexture, 0);

	UINT* heights = new UINT[readDesc.Width * readDesc.Height];
	for (UINT i = 0; i < readDesc.Width * readDesc.Height; i++)
	{
		UINT pixel = pixels[i];
		UINT result = 0;

		Channel channel = (Channel)selectedChannel;
		switch (channel)
		{
			case Editor::Channel::Alpha: result = (0xFF000000 & pixel) >> 24; break;
			case Editor::Channel::Blue:	 result = (0x00FF0000 & pixel) >> 16; break;
			case Editor::Channel::Green: result = (0x0000FF00 & pixel) >> 8;  break;
			case Editor::Channel::Red:	 result = (0x000000FF & pixel) >> 0;  break;
		}

		heights[i] = result << 24;
		
	}

	//위에서 저장된 높이값을 텍스쳐에 써주기
	ID3D11Texture2D* saveTexture;
	D3D11_TEXTURE2D_DESC saveDesc;
	ZeroMemory(&saveDesc, sizeof(D3D11_TEXTURE2D_DESC));
	saveDesc.Width = readDesc.Width;
	saveDesc.Height = readDesc.Height;
	saveDesc.ArraySize = 1;
	saveDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	saveDesc.MipLevels = 1;
	saveDesc.SampleDesc = readDesc.SampleDesc;
	saveDesc.Usage = D3D11_USAGE_STAGING;
	saveDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA saveSubResource;
	saveSubResource.pSysMem = heights;
	saveSubResource.SysMemPitch = sizeof(UINT) * readDesc.Width;
	saveSubResource.SysMemSlicePitch = sizeof(UINT) * readDesc.Width * readDesc.Height;
	Check(D3D::GetDevice()->CreateTexture2D(&saveDesc, &saveSubResource, &saveTexture));

	wstring fileName = Path::GetFileNameWithoutExtension(heightMapFile->GetFile());
	fileName = L"../../_Textures/Terrain/" + fileName + L".dds";
	D3DX11SaveTextureToFile(D3D::GetDC(), saveTexture, D3DX11_IFF_DDS, fileName.c_str());

	SafeRelease(srcTexture);
	SafeRelease(readTexture);
	SafeRelease(saveTexture);

}

void Editor::UpdateDataMapFileList()
{
	dataMapList.clear();
	Path::GetFiles(&dataMapList, L"../../_Textures/Terrain/", L"*.dds", false);

	for (wstring& file : dataMapList)
		file = Path::GetFileNameWithoutExtension(file);
}
