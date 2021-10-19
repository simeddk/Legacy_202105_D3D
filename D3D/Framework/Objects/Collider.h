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
private:
	struct Bounding;

public:
	Collider(Transform* transform, Transform* init = nullptr);
	~Collider();

	void Update();
	void Render(Color color = Color(0, 1, 0, 1));

	bool Intersection(Collider* other);

private:
	bool SeparatePlane(Vector3& distance, Vector3& direction, Bounding& box1, Bounding& box2);
	bool Collision(Bounding& box1, Bounding& box2);
	Vector3 Cross(Vector3& vec1, Vector3& vec2);
	

private:
	struct Bounding
	{
		Vector3 Position;

		Vector3 AxisX;
		Vector3 AxisY;
		Vector3 AxisZ;

		Vector3 HalfSize;
	} bounding;


private:
	Transform* init = nullptr;
	Transform* transform;

	Vector3 lines[8];
};