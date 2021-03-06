#pragma once
#include "Timer.h"

class CShader;
class CScene{
public:
	CScene();
	~CScene();

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects(const ComPtr<ID3D12Device>& device, ID3D12GraphicsCommandList* commandList);
	void ReleaseObjects();

	bool ProcessInput();
	void AnimateObjects(float fTimeElapsed);
	void Render(const ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList);

	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* device);
	ID3D12RootSignature* GetGraphicsRootSignature();

	void ReleaseUploadBuffers();

protected:
	CShader** shaders{ nullptr };
	int shaderCount{};

	ComPtr<ID3D12RootSignature> m_pd3dGraphicsRootSignature;
	ComPtr<ID3D12PipelineState> m_pd3dPipelineState;

};

