#pragma once

class CGameObject;

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
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* commandList);

	virtual void ReleaseShaderVariables() {}
	virtual void ReleaseUploadBuffers();

	virtual void BuildObjects(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, void* context = nullptr);
	virtual void AnimateObjects(float elapsedTime);
	virtual void ReleaseObjects();

	virtual void OnPrepareRender(ID3D12GraphicsCommandList* commandList);
	virtual void Render(ID3D12GraphicsCommandList* commandList);

protected:
	// 셰이더가 포함하는 게임 객체의 리스트 
	CGameObject** objects{ nullptr };
	int objectCount{ 0 };

	//파이프라인 상태 객체들의 리스트
	ID3D12PipelineState** pipelineStates{ nullptr };
	int pipelineStateCount{ 0 };
private:
	int ref{};
};

