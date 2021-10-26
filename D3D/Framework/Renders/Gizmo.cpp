#include "Framework.h"
#include "Gizmo.h"

Gizmo* Gizmo::instance = nullptr;

void Gizmo::Create()
{
	assert(instance == nullptr);

	instance = new Gizmo();
}

void Gizmo::Delete()
{
	SafeDelete(instance);
}

Gizmo * Gizmo::Get()
{
	return instance;
}

Gizmo::Gizmo()
{
}

Gizmo::~Gizmo()
{
}

void Gizmo::Update()
{
}

void Gizmo::Render()
{
	if (transform == nullptr) return;

	ImGuizmo::SetOrthographic(false);
	ImGuizmo::BeginFrame();
	ImGuizmo::SetID(0);

	if (Mouse::Get()->Press(1) == false)
	{
		if (Keyboard::Get()->Press(VK_SHIFT))
		{
			if (Keyboard::Get()->Down('W'))
				operation = ImGuizmo::TRANSLATE;
			else if (Keyboard::Get()->Down('E'))
				operation = ImGuizmo::ROTATE;
			else if (Keyboard::Get()->Down('R'))
				operation = ImGuizmo::SCALE;
			else if (Keyboard::Get()->Down('Q'))
				mode = (ImGuizmo::MODE)!mode;
		}
	}

	Matrix V, P;
	Context::Get()->GetCamera()->GetMatrix(&V);
	Context::Get()->GetPerspective()->GetMatrix(&P);

	ImGuizmo::SetRect(0, 0, D3D::Width(), D3D::Height());

	Matrix W = transform->World();
	ImGuizmo::Manipulate(V, P, operation, mode, W);

	transform->World(W);
}

void Gizmo::SetTransform(Transform * transform)
{
	this->transform = transform;
}
