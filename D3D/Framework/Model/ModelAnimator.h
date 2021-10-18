#pragma once

class ModelAnimator
{
public:
	ModelAnimator(Shader* shader);
	~ModelAnimator();

	void Update();
	void Render();

private:
	void UpdateAnimationFrame(UINT instance);
	void UpdateBlendFrame(UINT instance);

public:
	void ReadMaterial(wstring file);
	void ReadMesh(wstring file);
	void ReadClip(wstring file);

	void PlayTweenMode(UINT instance, UINT clip, float speed = 1.0f, float takeTime = 1.0f);
	void PlayBlendMode(UINT instance, UINT clip1, UINT clip2, UINT clip3);
	void SetBlendAlpha(UINT instance, float alpha);


public:
	void Pass(UINT pass);

	Transform* AddTransform();
	Transform* GetTransform(UINT index){ return transforms[index]; }
	UINT TransformCount() { return transforms.size(); }
	void UpdateTransforms();

	void SetColor(UINT instance, Color& color);


	Model* GetModel() { return model; }

	void GetAttachBones(UINT instance, Matrix* matrix);
	
private:
	void CreateTexture();
	void CreateClipTransform(UINT index);

private:
	struct ClipTransform
	{
		Matrix** Transform;
	
		ClipTransform()
		{
			Transform = new Matrix*[MAX_MODEL_KEYFRAMES];

			for (UINT i = 0; i < MAX_MODEL_KEYFRAMES; i++)
				Transform[i] = new Matrix[MAX_MODEL_TRANSFORMS];
		}

		~ClipTransform()
		{
			for (UINT i = 0; i < MAX_MODEL_KEYFRAMES; i++)
				SafeDeleteArray(Transform[i]);

			SafeDeleteArray(Transform);
		}
	};
	ClipTransform* clipTransforms;

private:
	ID3D11Texture2D* texture = nullptr;
	ID3D11ShaderResourceView* transformSRV;
	ID3DX11EffectShaderResourceVariable* sTransformSRV;

private:
	struct KeyFrameDesc
	{
		int Clip = 0;

		UINT CurrFrame = 0;
		UINT NextFrame = 0;

		float Time = 0.0f;
		float RunningTime = 0.0f;

		float Speed = 1.0f;

		Vector2 Padding;
	};

private:
	struct TweenDesc
	{
		float TakeTime = 0.1f;
		float TweenTime = 0.0f;
		float ChangeTime = 0.0f;
		float Padding;

		KeyFrameDesc Curr;
		KeyFrameDesc Next;
		
		TweenDesc()
		{
			Curr.Clip = 0;
			Next.Clip = -1;
		}

	} tweenDesc[MAX_MODEL_INSTANCE];

	ConstantBuffer* frameBuffer;
	ID3DX11EffectConstantBuffer* sFrameBuffer;

private:
	struct BlendDesc
	{
		UINT Mode = 0;
		float Alpha = 0;
		Vector2 Padding;

		KeyFrameDesc Clips[3];
	} blendDesc[MAX_MODEL_INSTANCE];

	ConstantBuffer* blendBuffer;
	ID3DX11EffectConstantBuffer* sBlendBuffer;
	

private:
	Shader* shader;
	Model* model;

	vector<Transform*> transforms;
	Matrix worlds[MAX_MODEL_INSTANCE];
	VertexBuffer* instanceBuffer;

	Color colors[MAX_MODEL_INSTANCE];
	VertexBuffer* instanceColorBuffer;

private:
	float frameRate = 30.0f;
	float frameTime = 0.0f;

	Shader* computeShader;

	Matrix** attachBones;

	ID3DX11EffectMatrixVariable* sComputeWorld;
	ID3DX11EffectConstantBuffer* sComputeFrameBuffer;
	ID3DX11EffectConstantBuffer* sComputeBlendBuffer;
	ID3DX11EffectShaderResourceVariable* sComputeTransformSRV;

	StructuredBuffer* computeWorldBuffer;
	ID3DX11EffectShaderResourceVariable* sComputeInputWorldBuffer;

	StructuredBuffer* computeBoneBuffer;
	ID3DX11EffectShaderResourceVariable* sComputeInputBoneBuffer;

	TextureBuffer* computeOutputBuffer;
	ID3DX11EffectUnorderedAccessViewVariable* sComputeOutputBuffer;
};