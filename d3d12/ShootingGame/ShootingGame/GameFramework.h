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
	// 렌더타겟, 서술자힙 포인터. 렌더 타겟 서술자 크기

	ComPtr<ID3D12Resource> m_pd3dDepthStencilBuffer;
	ComPtr<ID3D12DescriptorHeap> m_pd3dDsvDescriptorHeap;
	UINT m_nDsvDescriptorIncrementSize;
	// 깊이 스텐실 버퍼, 서술자 힙 포인터. 깊이 스텐실 서술자 크기

	ComPtr<ID3D12CommandQueue> m_pd3dCommandQueue;
	ComPtr<ID3D12CommandAllocator> m_pd3dCommandAllocator;
	ComPtr<ID3D12GraphicsCommandList> m_pd3dCommandList;
	//명령 큐, 명령 할당자, 명령 리스트 포인터

	ComPtr<ID3D12PipelineState> m_pd3dPipelineState;
	// 그래픽 파이프라인 상태 객체 포인터

	ComPtr<ID3D12Fence> m_pd3dFence;
	UINT64 m_nFenceValue;
	HANDLE m_hFenceEvent;
	// 펜스 포인터, 펜스 값, 이벤트 핸들

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
	//스왑 체인, 디바이스, 서술자 힙, 명령 큐/할당자/리스트 생성 함수

	void CreateRenderTargetViews();
	void CreateDepthStencilView();
	//렌더 타겟 뷰와 깊이 스텐실 뷰 생성 함수

	void BuildObjects();
	void ReleaseObjects();
	//렌더링할 메쉬와 게임 객체를 생성하고 소멸하는 함수

	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();
	// 사용자 입력, 애니메이션, 렌더링 함수

	void WaitForGpuComplete();
	// CPU와 GPU 동기화 함수

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);

	//윈도우의 메세지 처리 함수

	CGameTimer gameTimer{};
	_TCHAR pszFrameRate[50];

	// 게임 타이머

	void changeSwapChainState();


	CCamera* m_pCamera{ nullptr };
	//전체화면모드
};

