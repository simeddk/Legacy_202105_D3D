#pragma once

//-----------------------------------------------------------------------------
//ColliderObject
//-----------------------------------------------------------------------------
struct ColliderObject
{
public:
	class Transform* Init = nullptr;
	class Transform* World = nullptr;
	class Collider* Collision = nullptr;

public:
	ColliderObject();
	~ColliderObject();
};


//-----------------------------------------------------------------------------
//Collider
//-----------------------------------------------------------------------------
class Collider
{
public:
	Collider(Transform* transform, Transform* init = nullptr);
	~Collider();

	void Update();
	void Render(Color color = Color(0, 1, 0, 1));

private:
	Transform* init = nullptr;
	Transform* transform;

	Vector3 lines[8];
};