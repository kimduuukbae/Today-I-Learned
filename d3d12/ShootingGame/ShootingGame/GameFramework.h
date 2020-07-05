#pragma once
#include "Timer.h"
#include "Scene.h"
#include "Player.h"

#pragma comment(lib, "fmod_vc")

namespace FMOD {
	class System;
	class Channel;
	class ChannelGroup;
	class Sound;
}
class CCamera;

class CGameFramework{
private:
	HINSTANCE m_hInstance;
	HWND m_hWnd;

	int m_nWndClientWidth;
	int m_nWndClientHeight;

	ComPtr<IDXGIFactory4> m_pdxgiFactory;
	ComPtr<IDXGISwapChain3> m_pdxgiSwapChain;
	ComPtr<ID3D12Device> m_pd3dDevice;

	bool m_bMsaa4xEnable { false };
	UINT m_nMsaa4xQualityLevels { 0 };
	static const UINT m_nSwapChainBuffers { 2 };
	UINT m_nSwapChainBufferIndex;

	ComPtr<ID3D12Resource> m_ppd3dRenderTargetBuffers[m_nSwapChainBuffers];
	ComPtr<ID3D12DescriptorHeap> m_pd3dRtvDescriptorHeap;
	UINT m_nRtvDescriptorIncrementSize;
	// ����Ÿ��, �������� ������. ���� Ÿ�� ������ ũ��

	ComPtr<ID3D12Resource> m_pd3dDepthStencilBuffer;
	ComPtr<ID3D12DescriptorHeap> m_pd3dDsvDescriptorHeap;
	UINT m_nDsvDescriptorIncrementSize;
	// ���� ���ٽ� ����, ������ �� ������. ���� ���ٽ� ������ ũ��

	ComPtr<ID3D12CommandQueue> m_pd3dCommandQueue;
	ComPtr<ID3D12CommandAllocator> m_pd3dCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_pd3dCommandList;
	//��� ť, ��� �Ҵ���, ��� ����Ʈ ������

	ComPtr<ID3D12PipelineState> m_pd3dPipelineState;
	// �׷��� ���������� ���� ��ü ������

	ComPtr<ID3D12Fence> m_pd3dFence;
	UINT64 m_nFenceValue;
	HANDLE m_hFenceEvent;
	// �潺 ������, �潺 ��, �̺�Ʈ �ڵ�

	CScene* m_pScene;
	CPlayer* m_pPlayer;
	POINT m_ptOldCursorPos;

	FMOD::System* pFmod;
	FMOD::Channel* ch[3];
	FMOD::ChannelGroup* gr[3];
	FMOD::Sound* Sound[3];

public:
	CGameFramework();
	~CGameFramework();

	bool OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateDirect3DDevice();
	void CreateCommandQueueAndList();
	//���� ü��, ����̽�, ������ ��, ��� ť/�Ҵ���/����Ʈ ���� �Լ�

	void CreateRenderTargetViews();
	void CreateDepthStencilView();
	//���� Ÿ�� ��� ���� ���ٽ� �� ���� �Լ�

	void BuildObjects();
	void ReleaseObjects();
	//�������� �޽��� ���� ��ü�� �����ϰ� �Ҹ��ϴ� �Լ�

	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();
	// ����� �Է�, �ִϸ��̼�, ������ �Լ�

	void WaitForGpuComplete();
	// CPU�� GPU ����ȭ �Լ�

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	//�������� �޼��� ó�� �Լ�

	CGameTimer gameTimer{};
	_TCHAR pszFrameRate[50];

	// ���� Ÿ�̸�

	void changeSwapChainState();


	CCamera* m_pCamera{ nullptr };
	//��üȭ����
};

