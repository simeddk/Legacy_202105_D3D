#pragma once
class Math
{
public:
	static const float PI;
	static const float EPSILON;

	static float Modulo(float val1, float val2);

	static float ToRadian(float degree);
	static float ToDegree(float radian);

	static int Random(int r1, int r2);
	static float Random(float r1, float r2);
	
	static D3DXVECTOR2 RandomVec2(float r1, float r2);
	static D3DXVECTOR3 RandomVec3(float r1, float r2);
	static D3DXCOLOR RandomColor3();
	static D3DXCOLOR RandomColor4();


	static void LerpMatrix(OUT D3DXMATRIX& out, const D3DXMATRIX& m1, const D3DXMATRIX& m2, float amount);

	static D3DXQUATERNION LookAt(const D3DXVECTOR3& origin, const D3DXVECTOR3& target, const D3DXVECTOR3& up);
	static float Gaussian(float val, UINT blurCount);

	static void MatrixDecompose(const D3DXMATRIX& m, OUT Vector3& S, OUT Vector3& R, OUT Vector3& T);

	template<typename T>
	static T Clamp(T value, T min, T max)
	{
		value = value > max ? max : value;
		value = value < min ? min : value;
		return value;
	}

	template<typename T>
	static T Lerp(T a, T b, T alpha)
	{
		//return (1 - alpha) * a + alpha * b;
		return a + (b - a) * alpha;
	}
};