#pragma once

class ModelAnimator
{
public:
	ModelAnimator(Shader* shader);
	~ModelAnimator();

	void Update();
	void Render();

private:
	void UpdateAnimationFrame();
	void UpdateBledningFrame();

public:
	void ReadMaterial(wstring file);
	void ReadMesh(wstring file);
	void ReadClip(wstring file);

	void PlayTweenMode(UINT clip, float speed = 1.0f, float takeTime = 1.0f);
	void PlayBlendMode(UINT clip1, UINT clip2, UINT clip3);
	void SetBlendAlpha(float alpha);


public:
	void SetShader(Shader* shader, bool bFirst = false);
	void Pass(UINT pass);
	Transform* GetTransform(){ return transform; }
	Model* GetModel() { return model; }

	void GetAttachBones(Matrix* matrix);
	
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
		float TakeTime = 1.0f;
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

	} tweenDesc;

	ConstantBuffer* frameBuffer;
	ID3DX11EffectConstantBuffer* sFrameBuffer;

private:
	struct BlendDesc
	{
		UINT Mode = 0;
		float Alpha = 0;
		Vector2 Padding;

		KeyFrameDesc Clips[3];
	} blendDesc;

	ConstantBuffer* blendBuffer;
	ID3DX11EffectConstantBuffer* sBlendBuffer;
	

private:
	Shader* shader;
	Model* model;

	Transform* transform;

private:
	float frameRate = 30.0f;
	float frameTime = 0.0f;

	Shader* computeShader;

	ID3DX11EffectMatrixVariable* sComputeWorld;
	ID3DX11EffectConstantBuffer* sComputeFrameBuffer;
	ID3DX11EffectConstantBuffer* sComputeBlendBuffer;
	ID3DX11EffectShaderResourceVariable* sComputeTransformSRV;

	StructuredBuffer* computeBoneBuffer;
	ID3DX11EffectShaderResourceVariable* sComputeInputBoneBuffer;
	ID3DX11EffectUnorderedAccessViewVariable* sComputeOutputBoneBuffer;
};