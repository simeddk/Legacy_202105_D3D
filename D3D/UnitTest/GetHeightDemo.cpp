#include "stdafx.h"
#include "GetHeightDemo.h"

void GetHeightDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(6, 0, 0);
	Context::Get()->GetCamera()->Position(110, 50, -110);
	dynamic_cast<Freedom*>(Context::Get()->GetCamera())->Speed(50, 2);

	terrainShader = new Shader(L"08_Terrain.fxo");
	terrain = new Terrain(terrainShader, L"Terrain/Gray256.png");
	terrain->BaseMap(L"Terrain/Cliff (Sandstone).jpg");

	triangleShader = new Shader(L"04_World.fxo");
	Vertex vertices[3];
	vertices[0].Position = Vector3(0, 1, 0);
	vertices[1].Position = Vector3(-1, 0, 0);
	vertices[2].Position = Vector3(+1, 0, 0);

	//Create VertexBuffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(Vertex) * 3;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = vertices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
	}

}

void GetHeightDemo::Destroy()
{
	SafeDelete(terrainShader);
	SafeDelete(triangleShader);
	
	SafeDelete(terrain);

	SafeRelease(vertexBuffer);
}

void GetHeightDemo::Update()
{
	static UINT pass = terrainShader->PassCount() - 1;
	ImGui::InputInt("Pass", (int*)&pass);
	pass %= terrainShader->PassCount();
	terrain->Pass() = pass;

	static Vector3 LightDirection = Vector3(-1, -1, 1);
	ImGui::SliderFloat3("LightDirection", (float*)&LightDirection, -1, 1);
	terrainShader->AsVector("LightDirection")->SetFloatVector(LightDirection);

	static UINT Albedo = 1;
	ImGui::InputInt("Albedo", (int*)&Albedo);
	Albedo %= 2;
	terrainShader->AsScalar("Albedo")->SetInt(Albedo);

	if (Keyboard::Get()->Press(VK_RIGHT))
		position.x += 20.0f * Time::Delta();
	else if (Keyboard::Get()->Press(VK_LEFT))
		position.x -= 20.0f * Time::Delta();

	if (Keyboard::Get()->Press(VK_UP))
		position.z += 20.0f * Time::Delta();
	else if (Keyboard::Get()->Press(VK_DOWN))
		position.z -= 20.0f * Time::Delta();

	//position.y = terrain->GetHeight(position) + 1.0f;
	position.y = terrain->GetHeight_Raycast(position) + 1.0f;

	Matrix R, T;
	D3DXMatrixRotationX(&R, Math::PI);
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);

	Matrix world;
	world = R * T;
	triangleShader->AsMatrix("World")->SetMatrix(world);
}


void GetHeightDemo::Render()
{
	triangleShader->AsMatrix("View")->SetMatrix(Context::Get()->View());
	triangleShader->AsMatrix("Projection")->SetMatrix(Context::Get()->Projection());
	triangleShader->AsVector("Color")->SetFloatVector(Color(1, 0, 0, 1));

	terrain->Render();

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	triangleShader->Draw(0, 2, 3);

	string str = to_string(position.x) + ", " + to_string(position.y) + ", " + to_string(position.z);
	Gui::Get()->RenderText(5, 60, str);
}



