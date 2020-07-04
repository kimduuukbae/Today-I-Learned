#pragma once

class CGameObject;
class CCamera;
struct CB_GAMEOBJECT_INFO {
	XMFLOAT4X4 worldMatrix{};
};
struct VS_VB_INSTANCE {
	XMFLOAT4X4 transform{};
	XMFLOAT4 color{};
};
class CShader{
public:
	CShader();
	~CShader();

	void AddRef() { ++ref; }
	void Release() { if (--ref <= 0) delete this; }

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** shaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** shaderBlob);
	
	D3D12_SHADER_BYTECODE CompileShaderFromFile(const WCHAR* fileName, LPCSTR shaderName, LPCSTR shaderProfile, ID3DBlob** shaderBlob);

	virtual void CreateShader(ID3D12Device* device, ID3D12RootSignature* rootSignature);
	virtual void CreateShaderVariables(ID3D12Device* device, ID3D12CommandList* commandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* commandList, XMFLOAT4X4* worldMatrix);

	virtual void ReleaseShaderVariables();

	virtual void OnPrepareRender(ID3D12GraphicsCommandList* commandList);
	virtual void Render(ID3D12GraphicsCommandList* commandList, CCamera* pCamera);

protected:

	//파이프라인 상태 객체들의 리스트
	ID3D12PipelineState** pipelineStates{ nullptr };
	int pipelineStateCount{ 0 };
private:
	int ref{};
};

class CPlayerShader : public CShader {
public:
	CPlayerShader();
	virtual ~CPlayerShader();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout() override;
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** shaderBlob) override;
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** shaderBlob) override;

	virtual void CreateShader(ID3D12Device* device, ID3D12RootSignature* rootSignature) override;
};

class CObjectsShader : public CShader {
public:
	CObjectsShader();
	virtual ~CObjectsShader();

	virtual void BuildObjects(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	virtual void AnimateObjects(float elapsedTime);
	virtual void ReleaseObjects();

	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout() override;
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** shaderBlob) override;
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** shaderBlob) override;

	virtual void CreateShader(ID3D12Device* device, ID3D12RootSignature* rootSignature) override;
	virtual void ReleaseUploadBuffers();
	
	virtual void Render(ID3D12GraphicsCommandList* commandList, CCamera* pCamera);

protected:
	CGameObject** m_ppObjects{ nullptr };
	int m_nObjects{ 0 };
};

class CInstancingShader : public CObjectsShader
{
public:
	CInstancingShader();
	virtual ~CInstancingShader();
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob** ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob** ppd3dShaderBlob);
	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature
		* pd3dGraphicsRootSignature);
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void ReleaseShaderVariables();
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);
protected:
	//인스턴스 데이터를 포함하는 버퍼와 포인터이다. 
	ID3D12Resource *m_pd3dcbGameObjects = NULL;
	VS_VB_INSTANCE* m_pcbMappedGameObjects = NULL;
};