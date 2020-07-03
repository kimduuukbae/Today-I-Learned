#pragma once
#include "Timer.h"
#include "Shader.h"

class CGameObject;
class CCamera;

class CScene{
public:
	CScene();
	~CScene();

	bool OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	void BuildObjects(const ComPtr<ID3D12Device>& device, ID3D12GraphicsCommandList* commandList);
	void ReleaseObjects();

	bool ProcessInput(UCHAR* pKeysBuffer);
	bool ProcessInput();
	void AnimateObjects(float fTimeElapsed);
	void Render(const ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, CCamera* pCamera);

	ID3D12RootSignature* CreateGraphicsRootSignature(ID3D12Device* device);
	ID3D12RootSignature* GetGraphicsRootSignature();

	void ReleaseUploadBuffers();

protected:
	ComPtr<ID3D12RootSignature> m_pd3dGraphicsRootSignature;
	ComPtr<ID3D12PipelineState> m_pd3dPipelineState;

	CObjectsShader* m_pShaders{ nullptr };
	int m_nShaders{};
};

