#include "stdafx.h"
#include "World2Demo.h"

void World2Demo::Initialize()
{
	shader = new Shader(L"04_World.fxo");

	//Tri1
	/*vertices[0].Position = Vector3(-0.5f, -0.5f, 0);
	vertices[1].Position = Vector3(-0.5f, +0.5f, 0);
	vertices[2].Position = Vector3(+0.5f, -0.5f, 0);
	vertices[3].Position = Vector3(+0.5f, -0.5f, 0);
	vertices[4].Position = Vector3(-0.5f, +0.5f, 0);
	vertices[5].Position = Vector3(+0.5f, +0.5f, 0);*/

	vertices[0].Position = Vector3(-0.5f + 0.5f, -0.5f + 0.5f, 0);
	vertices[1].Position = Vector3(-0.5f + 0.5f, +0.5f + 0.5f, 0);
	vertices[2].Position = Vector3(+0.5f + 0.5f, -0.5f + 0.5f, 0);
	vertices[3].Position = Vector3(+0.5f + 0.5f, -0.5f + 0.5f, 0);
	vertices[4].Position = Vector3(-0.5f + 0.5f, +0.5f + 0.5f, 0);
	vertices[5].Position = Vector3(+0.5f + 0.5f, +0.5f + 0.5f, 0);

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = sizeof(Vertex) * 6;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subResource = { 0 };
	subResource.pSysMem = vertices;

	Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));

	for (UINT i = 0; i < 10; i++)
	{
		Vector3 scale = Math::RandomVec3(0.5f, 2.0f);
		Vector3 position = Math::RandomVec3(-3.0f, 3.0f);

		Matrix S, T;
		D3DXMatrixScaling(&S, scale.x, scale.y, scale.z);
		D3DXMatrixTranslation(&T, position.x, position.y, position.z);

		worlds[i] = S * T;
		colors[i] = Math::RandomColor4();
	}
}

void World2Demo::Destroy()
{
	SafeDelete(shader);
	SafeRelease(vertexBuffer);
}

void World2Demo::Update()
{
	//Select Index
	static UINT index = 0;
	
	if (Keyboard::Get()->Down(VK_ADD)) index++;
	else if (Keyboard::Get()->Down(VK_SUBTRACT)) index--;

	index = Math::Clamp<UINT>(index, 0, ARRAYSIZE(worlds) - 1);

	//Draw ImGui
	ImVec4 textColor = ImVec4(colors[index].r, colors[index].g, colors[index].b, 1.0f);
	ImGui::TextColored(textColor, "[%d]", index);

	Vector2 position = Vector2(worlds[index]._41, worlds[index]._42);
	Vector2 scale = Vector2(worlds[index]._11, worlds[index]._22);
	ImGui::TextColored(textColor, "Position : %0.2f\t %0.2f\t %0.2f", position.x, position.y);
	ImGui::TextColored(textColor, "Scale : %0.2f\t %0.2f\t %0.2f", scale.x, scale.y);

	//Scale
	if (Keyboard::Get()->Press(VK_LSHIFT))
	{
		if (Keyboard::Get()->Press('D'))
			worlds[index]._11 += 2.0f * Time::Delta();
		else if (Keyboard::Get()->Press('A'))
			worlds[index]._11 -= 2.0f * Time::Delta();

		if (Keyboard::Get()->Press('W'))
			worlds[index]._22 += 2.0f * Time::Delta();
		else if (Keyboard::Get()->Press('S'))
			worlds[index]._22 -= 2.0f * Time::Delta();

		return;
	}

	//Translation
	if (Keyboard::Get()->Press('D'))
		worlds[index]._41 += 2.0f * Time::Delta();
	else if (Keyboard::Get()->Press('A'))
		worlds[index]._41 -= 2.0f * Time::Delta();

	if (Keyboard::Get()->Press('W'))
		worlds[index]._42 += 2.0f * Time::Delta();
	else if (Keyboard::Get()->Press('S'))
		worlds[index]._42 -= 2.0f * Time::Delta();

	//Rotation
	if (Keyboard::Get()->Press('Q'))
	{
		for (auto& w : worlds)
		{
			Vector3 scale, rotation, position;
			Math::MatrixDecompose(w, scale, rotation, position);

			rotation.z += 10.0f * Time::Delta();

			Matrix S, R, T;
			D3DXMatrixScaling(&S, scale.x, scale.y, scale.z);
			D3DXMatrixTranslation(&T, position.x, position.y, position.z);
			D3DXMatrixRotationZ(&R, rotation.z);
			
			w = S * R * T;
		}
	}
}


void World2Demo::Render()
{
	
	shader->AsMatrix("View")->SetMatrix(Context::Get()->View());
	shader->AsMatrix("Projection")->SetMatrix(Context::Get()->Projection());

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	static UINT pass = 0;
	ImGui::InputInt("Pass", (int*)&pass);
	pass %= 2;

	for (UINT i = 0; i < 10; i++)
	{
		shader->AsVector("Color")->SetFloatVector(colors[i]);
		shader->AsMatrix("World")->SetMatrix(worlds[i]);
		shader->Draw(0, pass, 6);
	}
}



