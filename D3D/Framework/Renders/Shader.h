#pragma once

class Shader
{
public:
	friend struct Pass;

public:
	Shader(wstring file);
	~Shader();

	wstring GetFile() { return file; }

	ID3DX11Effect* Effect() { return effect; }

	void Draw(UINT technique, UINT pass, UINT vertexCount, UINT startVertexLocation = 0);
	void DrawIndexed(UINT technique, UINT pass, UINT indexCount, UINT startIndexLocation = 0, INT baseVertexLocation = 0);
	void DrawInstanced(UINT technique, UINT pass, UINT vertexCountPerInstance, UINT instanceCount, UINT startVertexLocation = 0, UINT startInstanceLocation = 0);
	void DrawIndexedInstanced(UINT technique, UINT pass, UINT indexCountPerInstance, UINT instanceCount, UINT startIndexLocation = 0, INT baseVertexLocation = 0, UINT startInstanceLocation = 0);

	void Dispatch(UINT technique, UINT pass, UINT x, UINT y, UINT z);

	UINT PassCount(UINT techIndex = 0) { return techniques[techIndex].Passes.size(); }


	ID3DX11EffectVariable* Variable(string name);
	ID3DX11EffectScalarVariable* AsScalar(string name);
	ID3DX11EffectVectorVariable* AsVector(string name);
	ID3DX11EffectMatrixVariable* AsMatrix(string name);
	ID3DX11EffectStringVariable* AsString(string name);
	ID3DX11EffectShaderResourceVariable* AsSRV(string name);
	ID3DX11EffectRenderTargetViewVariable* AsRTV(string name);
	ID3DX11EffectDepthStencilViewVariable* AsDSV(string name);
	ID3DX11EffectUnorderedAccessViewVariable* AsUAV(string name);
	ID3DX11EffectConstantBuffer* AsConstantBuffer(string name);
	ID3DX11EffectShaderVariable* AsShader(string name);
	ID3DX11EffectBlendVariable* AsBlend(string name);
	ID3DX11EffectDepthStencilVariable* AsDepthStencil(string name);
	ID3DX11EffectRasterizerVariable* AsRasterizer(string name);
	ID3DX11EffectSamplerVariable* AsSampler(string name);


private:
	void CreateEffect();
	ID3D11InputLayout* CreateInputLayout(ID3DBlob* fxBlob, D3DX11_EFFECT_SHADER_DESC* effectVsDesc, vector<D3D11_SIGNATURE_PARAMETER_DESC>& params);

private:
	wstring file;

	ID3DX11Effect* effect;
	D3DX11_EFFECT_DESC effectDesc;

private:
	struct StateBlock
	{
		ID3D11RasterizerState * RSRasterizerState;

		ID3D11BlendState* OMBlendState;
		FLOAT OMBlendFactor[4];
		UINT OMSampleMask;
		ID3D11DepthStencilState* OMDepthStencilState;
		UINT OMStencilRef;
	};
	StateBlock* initialStateBlock;

private:
	struct Pass
	{
		wstring Name;
		ID3DX11EffectPass* IPass;
		D3DX11_PASS_DESC Desc;

		ID3D11InputLayout* InputLayout;
		D3DX11_PASS_SHADER_DESC PassVsDesc;
		D3DX11_EFFECT_SHADER_DESC EffectVsDesc;
		vector<D3D11_SIGNATURE_PARAMETER_DESC> SignatureDescs;

		D3DX11_STATE_BLOCK_MASK StateBlockMask;
		StateBlock* StateBlock;

		void Draw(UINT vertexCount, UINT startVertexLocation = 0);
		void DrawIndexed(UINT indexCount, UINT startIndexLocation = 0, INT baseVertexLocation = 0);
		void DrawInstanced(UINT vertexCountPerInstance, UINT instanceCount, UINT startVertexLocation = 0, UINT startInstanceLocation = 0);
		void DrawIndexedInstanced(UINT indexCountPerInstance, UINT instanceCount, UINT startIndexLocation = 0, INT baseVertexLocation = 0, UINT startInstanceLocation = 0);

		void BeginDraw();
		void EndDraw();

		void Dispatch(UINT x, UINT y, UINT z);
	};

	struct Technique
	{
		wstring Name;
		D3DX11_TECHNIQUE_DESC Desc;
		ID3DX11EffectTechnique* ITechnique;

		vector<Pass> Passes;

		void Draw(UINT pass, UINT vertexCount, UINT startVertexLocation = 0);
		void DrawIndexed(UINT pass, UINT indexCount, UINT startIndexLocation = 0, INT baseVertexLocation = 0);
		void DrawInstanced(UINT pass, UINT vertexCountPerInstance, UINT instanceCount, UINT startVertexLocation = 0, UINT startInstanceLocation = 0);
		void DrawIndexedInstanced(UINT pass, UINT indexCountPerInstance, UINT instanceCount, UINT startIndexLocation = 0, INT baseVertexLocation = 0, UINT startInstanceLocation = 0);

		void Dispatch(UINT pass, UINT x, UINT y, UINT z);
	};
	vector<Technique> techniques;
};