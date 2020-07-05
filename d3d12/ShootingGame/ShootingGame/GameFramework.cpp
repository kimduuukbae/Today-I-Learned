#include "stdafx.h"
#include "GameFramework.h"
#include "Camera.h"
#include "inc/fmod.hpp"

CGameFramework::CGameFramework() : 
	m_nWndClientWidth{ FRAME_BUFFER_WIDTH },
	m_nWndClientHeight{ FRAME_BUFFER_HEIGHT }{
	_tcscpy_s(pszFrameRate, _T("ShootingGame("));
	m_pScene = nullptr;
	FMOD::System_Create(&pFmod);
	pFmod->init(3, FMOD_INIT_NORMAL, nullptr);
	pFmod->createSound("data/bgm.mp3", FMOD_LOOP_NORMAL, nullptr, &Sound[E_BGM]);
	pFmod->createSound("data/attack.wav", FMOD_LOOP_OFF, nullptr, &Sound[E_ATTACK]);
	pFmod->createSound("data/hit.mp3", FMOD_LOOP_OFF, nullptr, &Sound[E_HIT]);
}

CGameFramework::~CGameFramework() {
#if defined(_DEBUG)
	IDXGIDebug1* pdxgiDebug{ nullptr };
	DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pdxgiDebug));
	HRESULT hResult = pdxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);
	pdxgiDebug->Release();
	_CrtDumpMemoryLeaks();
#endif
	for (int i = 0; i < 3; ++i) {
		Sound[i]->release();
		gr[i]->release();
	}
	pFmod->release();
}

bool CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd){

	m_hInstance = hInstance;
	m_hWnd = hMainWnd;

	CreateDirect3DDevice();
	CreateCommandQueueAndList();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();
	CreateRenderTargetViews();
	CreateDepthStencilView();

	BuildObjects();
	pFmod->playSound(Sound[E_BGM], gr[E_BGM], false, &ch[E_BGM]);
	return true;
}

void CGameFramework::OnDestroy(){
	WaitForGpuComplete();
	ReleaseObjects();

	::CloseHandle(m_hFenceEvent);

	m_pdxgiSwapChain->SetFullscreenState(false, nullptr);
}

void CGameFramework::CreateSwapChain(){
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	m_nWndClientWidth = rcClient.right - rcClient.left;
	m_nWndClientHeight = rcClient.bottom - rcClient.top;

	DXGI_SWAP_CHAIN_DESC1 dxgiSwapChainDesc{};
	dxgiSwapChainDesc.Width = m_nWndClientWidth;
	dxgiSwapChainDesc.Height = m_nWndClientHeight;
	dxgiSwapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dxgiSwapChainDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	dxgiSwapChainDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.BufferCount = m_nSwapChainBuffers;
	dxgiSwapChainDesc.Scaling = DXGI_SCALING_NONE;
	dxgiSwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiSwapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
	dxgiSwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	DXGI_SWAP_CHAIN_FULLSCREEN_DESC dxgiSwapChainFullScreenDesc{};
	dxgiSwapChainFullScreenDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainFullScreenDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainFullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	dxgiSwapChainFullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	dxgiSwapChainFullScreenDesc.Windowed = true;

	m_pdxgiFactory->CreateSwapChainForHwnd(m_pd3dCommandQueue.Get(), m_hWnd, &dxgiSwapChainDesc,
		&dxgiSwapChainFullScreenDesc, nullptr, (IDXGISwapChain1**)m_pdxgiSwapChain.GetAddressOf());
	m_pdxgiFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

//#ifndef _WITH_SWAPCHAIN_FULLSCREEN_STATE
//	CreateRenderTargetViews();
//#endif
}

void CGameFramework::CreateDirect3DDevice(){
	HRESULT hResult;
	UINT nDXGIFactoryFlags = 0;

#if defined(_DEBUG)
	ID3D12Debug* pd3dDebugController = nullptr;
	hResult = D3D12GetDebugInterface(IID_PPV_ARGS(&pd3dDebugController));
	if (pd3dDebugController) {
		pd3dDebugController->EnableDebugLayer();
		pd3dDebugController->Release();
	}
	nDXGIFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

	hResult = ::CreateDXGIFactory2(nDXGIFactoryFlags, IID_PPV_ARGS(m_pdxgiFactory.GetAddressOf()));

	IDXGIAdapter1* pd3dAdapter{ nullptr };
	for (UINT i = 0; DXGI_ERROR_NOT_FOUND != m_pdxgiFactory->EnumAdapters1(i, &pd3dAdapter); ++i) {
		DXGI_ADAPTER_DESC1 dxgiAdapterDesc;
		pd3dAdapter->GetDesc1(&dxgiAdapterDesc);
		if (dxgiAdapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
		if(SUCCEEDED(D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(m_pd3dDevice.GetAddressOf())))) break;
	}
	if (!pd3dAdapter) {
		m_pdxgiFactory->EnumWarpAdapter(__uuidof(IDXGIFactory4), (void**)&pd3dAdapter);
		D3D12CreateDevice(pd3dAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(m_pd3dDevice.GetAddressOf()));
	}

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount = 4;
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;
	m_pd3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	m_nMsaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;
	m_bMsaa4xEnable = (m_nMsaa4xQualityLevels > 1) ? true : false;
	hResult = m_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_pd3dFence));
	m_nFenceValue = 0;

	m_hFenceEvent = ::CreateEvent(nullptr, false, false, nullptr);

	if (pd3dAdapter)
		pd3dAdapter->Release();
}

void CGameFramework::CreateCommandQueueAndList(){
	D3D12_COMMAND_QUEUE_DESC d3dCommandQueueDesc{};
	d3dCommandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	d3dCommandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	HRESULT hResult = m_pd3dDevice->CreateCommandQueue(&d3dCommandQueueDesc,
		IID_PPV_ARGS(&m_pd3dCommandQueue));

	hResult = m_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&m_pd3dCommandAllocator));

	hResult = m_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_pd3dCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_pd3dCommandList));
	hResult = m_pd3dCommandList->Close();
}

void CGameFramework::CreateRtvAndDsvDescriptorHeaps(){
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc{};
	d3dDescriptorHeapDesc.NumDescriptors = m_nSwapChainBuffers;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask = 0;

	HRESULT hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc,
		IID_PPV_ARGS(&m_pd3dRtvDescriptorHeap));
	m_nRtvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	d3dDescriptorHeapDesc.NumDescriptors = 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hResult = m_pd3dDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc,
		IID_PPV_ARGS(&m_pd3dDsvDescriptorHeap));
	m_nDsvDescriptorIncrementSize = m_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

}
void CGameFramework::CreateRenderTargetViews() {
	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle
	{ m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
		
	for (UINT i = 0; i < m_nSwapChainBuffers; ++i) {
		m_pdxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_ppd3dRenderTargetBuffers[i]));
		m_pd3dDevice->CreateRenderTargetView(m_ppd3dRenderTargetBuffers[i].Get(), nullptr,
			d3dRtvCPUDescriptorHandle);
		d3dRtvCPUDescriptorHandle.ptr += m_nRtvDescriptorIncrementSize;
	}
}

void CGameFramework::CreateDepthStencilView(){
	D3D12_RESOURCE_DESC d3dResourceDesc{};
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = m_nWndClientWidth;
	d3dResourceDesc.Height = m_nWndClientHeight;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dResourceDesc.SampleDesc.Count = (m_bMsaa4xEnable) ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality = (m_bMsaa4xEnable) ? (m_nMsaa4xQualityLevels - 1) : 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES d3dHeapProperties{};

	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapProperties.CreationNodeMask = 1;
	d3dHeapProperties.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE d3dClearValue;
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;
	m_pd3dDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE,
		&d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue,
		IID_PPV_ARGS(&m_pd3dDepthStencilBuffer));

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCpuDescriptorHandle = 
		m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pd3dDevice->CreateDepthStencilView(m_pd3dDepthStencilBuffer.Get(), nullptr, d3dDsvCpuDescriptorHandle);

}

void CGameFramework::BuildObjects(){
	m_pd3dCommandList->Reset(m_pd3dCommandAllocator.Get(), NULL);
	m_pScene = new CScene();

	if (m_pScene) m_pScene->BuildObjects(m_pd3dDevice.Get(), m_pd3dCommandList.Get());
	CAirplanePlayer* pAirplanePlayer = new CAirplanePlayer(m_pd3dDevice.Get(),
		m_pd3dCommandList.Get(), m_pScene->GetGraphicsRootSignature());

	m_pPlayer = pAirplanePlayer;
	m_pCamera = m_pPlayer->GetCamera();

	m_pd3dCommandList->Close();
	ID3D12CommandList* ppd3dCommandLists[] = { m_pd3dCommandList.Get() };
	m_pd3dCommandQueue->ExecuteCommandLists(1, ppd3dCommandLists);

	WaitForGpuComplete();

	if (m_pScene) m_pScene->ReleaseUploadBuffers();
}

void CGameFramework::ReleaseObjects(){
	if (m_pScene) {
		m_pScene->ReleaseObjects();
		delete m_pScene;
	}
	if (m_pPlayer) {
		m_pPlayer->ReleaseShaderVariables();
		m_pPlayer->Release();
	}
}
void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam){
	switch (nMessageID) {
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		::ReleaseCapture();
		break;
	default:
		break;
	}
}

void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam){
	switch (nMessageID) {
	case WM_KEYUP:
		switch (wParam) {
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		case VK_F9:
			changeSwapChainState();
			break;
		case VK_CONTROL:
			reinterpret_cast<CAirplanePlayer*>(m_pPlayer)->ShootBullet();
			pFmod->playSound(Sound[E_ATTACK], gr[E_ATTACK], false, &ch[E_ATTACK]);
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

LRESULT CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam){
	switch (nMessageID) {
	case WM_SIZE: {
		m_nWndClientWidth = LOWORD(lParam);
		m_nWndClientHeight = HIWORD(lParam);
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return 0;
}

void CGameFramework::changeSwapChainState(){
	WaitForGpuComplete();

	BOOL bFullScreenState{ false };
	m_pdxgiSwapChain->GetFullscreenState(&bFullScreenState, nullptr);
	m_pdxgiSwapChain->SetFullscreenState(!bFullScreenState, nullptr);

	DXGI_MODE_DESC targetModeDesc{};
	targetModeDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	targetModeDesc.Width = m_nWndClientWidth;
	targetModeDesc.Height = m_nWndClientHeight;
	targetModeDesc.RefreshRate.Numerator = 60;
	targetModeDesc.RefreshRate.Denominator = 1;
	targetModeDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	targetModeDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	m_pdxgiSwapChain->ResizeTarget(&targetModeDesc);

	for (int i = 0; i < m_nSwapChainBuffers; ++i)
		m_ppd3dRenderTargetBuffers[i]->Release();

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	m_pdxgiSwapChain->GetDesc1(&swapChainDesc);
	m_pdxgiSwapChain->ResizeBuffers(m_nSwapChainBuffers, m_nWndClientWidth,
		m_nWndClientHeight, swapChainDesc.Format, swapChainDesc.Flags);

	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	CreateRenderTargetViews();
}

void CGameFramework::ProcessInput(){
	static UCHAR pKeyBuffer[256]{};
	DWORD dwDirection{};

	if (::GetKeyboardState(pKeyBuffer)){
		if (pKeyBuffer[VK_UP] & 0xF0) dwDirection |= DIR_FORWARD;
		if (pKeyBuffer[VK_DOWN] & 0xF0) dwDirection |= DIR_BACKWARD;
		if (pKeyBuffer[VK_LEFT] & 0xF0) dwDirection |= DIR_LEFT;
		if (pKeyBuffer[VK_RIGHT] & 0xF0) dwDirection |= DIR_RIGHT;
		if (pKeyBuffer[VK_PRIOR] & 0xF0) dwDirection |= DIR_UP;
		if (pKeyBuffer[VK_NEXT] & 0xF0) dwDirection |= DIR_DOWN;
	}

	float cxDelta{}, cyDelta{};
	POINT ptCursorPos{};

	if (::GetCapture() == m_hWnd){
		::SetCursor(NULL);
		::GetCursorPos(&ptCursorPos);
		cxDelta = (float)(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		cyDelta = (float)(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;
		::SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);

		if (pKeyBuffer[VK_RBUTTON] & 0xF0) {
			POINT ptPickingPos{ ptCursorPos };
			ScreenToClient(m_hWnd, &ptPickingPos);

			float xProjectPos{ 2.0f * ptPickingPos.x / (float)FRAME_BUFFER_WIDTH - 1.0f };
			float yProjectPos{ -2.0f * ptPickingPos.y / (float)FRAME_BUFFER_HEIGHT + 1.0f };

			XMFLOAT4X4 project{ m_pCamera->GetProjectionMatrix() };
			float xCamPos{ xProjectPos / project._11 };
			float yCamPos{ yProjectPos / project._22 };
			float zCamPos{ 1.0f };

			XMFLOAT4X4 viewInverse{ Matrix4x4::Inverse(m_pCamera->GetViewMatrix()) };

			XMFLOAT3 rayDir{ xCamPos, yCamPos, zCamPos }, rayDirOrigin{};
			XMStoreFloat3(&rayDirOrigin,
				XMVector3TransformCoord(XMLoadFloat3(&rayDirOrigin), XMLoadFloat4x4(&viewInverse)));
			XMStoreFloat3(&rayDir,
				XMVector3TransformNormal(XMLoadFloat3(&rayDir), XMLoadFloat4x4(&viewInverse)));
			rayDir = Vector3::Normalize(rayDir);
			if (auto ptr{ m_pScene->pickingObjects(rayDirOrigin, rayDir) }; ptr) {
				reinterpret_cast<CAirplanePlayer*>(m_pPlayer)->SetPickingTarget(ptr);
			}
		}
	}


	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f)){
		if (cxDelta || cyDelta){

			if (pKeyBuffer[VK_RBUTTON] & 0xF0) m_pPlayer->Rotate(cyDelta, 0.0f, cxDelta);
			else m_pPlayer->Rotate(cyDelta, cxDelta, 0.0f);
		}

		if (dwDirection) m_pPlayer->Move(dwDirection, 150.0f * gameTimer.getElapsedTime(), true);
	}


	m_pPlayer->Update(gameTimer.getElapsedTime());
	if (m_pScene->ProcessCollision(reinterpret_cast<CAirplanePlayer*>(m_pPlayer))) 
		pFmod->playSound(Sound[E_HIT], gr[E_HIT], false, &ch[E_HIT]);
	
}

void CGameFramework::AnimateObjects(){
	if (m_pScene) m_pScene->AnimateObjects(gameTimer.getElapsedTime());
}



void CGameFramework::FrameAdvance(){
	gameTimer.tick(0.0f);

	ProcessInput();
	AnimateObjects();

	HRESULT hResult = m_pd3dCommandAllocator->Reset();
	hResult = m_pd3dCommandList->Reset(m_pd3dCommandAllocator.Get(), nullptr);

	D3D12_RESOURCE_BARRIER d3dResourceBarrier{};
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier.Transition.pResource =
		m_ppd3dRenderTargetBuffers[m_nSwapChainBufferIndex].Get();
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle =
		m_pd3dRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	d3dRtvCPUDescriptorHandle.ptr += (m_nSwapChainBufferIndex * m_nRtvDescriptorIncrementSize);

	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle =
		m_pd3dDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	m_pd3dCommandList->OMSetRenderTargets(1, &d3dRtvCPUDescriptorHandle, true, &d3dDsvCPUDescriptorHandle);

	float pfClearColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_pd3dCommandList->ClearRenderTargetView(d3dRtvCPUDescriptorHandle,
		pfClearColor, 0, nullptr);

	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	//렌더링코드는 여기에
	m_pScene->Render(m_pd3dCommandList, m_pCamera);

#ifdef _WITH_PLAYER_TOP
	m_pd3dCommandList->ClearDepthStencilView(d3dDsvCPUDescriptorHandle, D3D12_CLEAR_FLAG_DEPTH |
		D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, NULL);
#endif
	if (m_pPlayer) m_pPlayer->Render(m_pd3dCommandList.Get(), m_pCamera);

	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	m_pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	hResult = m_pd3dCommandList->Close();

	ID3D12CommandList* ppd3dCommandLists[] { m_pd3dCommandList.Get() };
	m_pd3dCommandQueue->ExecuteCommandLists(_countof(ppd3dCommandLists), ppd3dCommandLists);
	
	WaitForGpuComplete();

	DXGI_PRESENT_PARAMETERS dxgiPresentParameters{};
	dxgiPresentParameters.DirtyRectsCount = 0;
	dxgiPresentParameters.pDirtyRects = nullptr;
	dxgiPresentParameters.pScrollRect = nullptr;
	dxgiPresentParameters.pScrollOffset = nullptr;
	m_pdxgiSwapChain->Present1(1, 0, &dxgiPresentParameters);
	//스왑체인 프리젠트 , 현재 후면버퍼가 전면버퍼로 이동하고, 렌더 타겟 인덱스가 바뀔것
	m_nSwapChainBufferIndex = m_pdxgiSwapChain->GetCurrentBackBufferIndex();

	gameTimer.getFrameRate(pszFrameRate + 13, 37);
	::SetWindowText(m_hWnd, pszFrameRate);
}

void CGameFramework::WaitForGpuComplete() {
	++m_nFenceValue;
	const UINT64 nFence = m_nFenceValue;
	HRESULT hResult = m_pd3dCommandQueue->Signal(m_pd3dFence.Get(), nFence);
	if (m_pd3dFence->GetCompletedValue() < nFence) {
		hResult = m_pd3dFence->SetEventOnCompletion(nFence, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}