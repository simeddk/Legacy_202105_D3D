#pragma once

class Gizmo
{
public:
	static void Create();
	static void Delete();
	static Gizmo* Get();

private:
	Gizmo();
	~Gizmo();

public:
	void Update();
	void Render();

public:
	void SetTransform(class Transform* transform);


private:
	static Gizmo* instance;

private:
	class Transform* transform = nullptr;

	ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
	ImGuizmo::MODE mode = ImGuizmo::WORLD;
};