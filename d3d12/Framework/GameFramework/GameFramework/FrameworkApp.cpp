#include "stdafx.h"
#include "FrameworkApp.h"
#include "SceneManager.h"
#include "ResourceManager.h"
#include "InputManager.h"
#include "GameplayStatics.h"
#include "d3dx12.h"
#include <iostream>

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace std;

FrameworkApp::FrameworkApp(HINSTANCE hInstance) : D3DApp{ hInstance } {}

FrameworkApp::~FrameworkApp() {
	sceneManager->Release();
	inputManager->Release();
}

bool FrameworkApp::Initialize() {
	if (!D3DApp::Initialize())
		return false;

	sceneManager = SceneManager::GetInstance();
	inputManager = InputManager::GetInstance();
	FailedAssert(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));
	sceneManager->Init();
	
	FailedAssert(mCommandList->Close());
	ID3D12CommandList* cmdsLists[]{ mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	FlushCommandQueue();

	sceneManager->ClearUploadBuffer();

	return true;
}

void FrameworkApp::OnResize() 
{
	D3DApp::OnResize();
	GameplayStatics::SetMainCameraFov(0.25f * XM_PI, AspectRatio(), 1.0f, 5000.0f);
}

void FrameworkApp::Update(const GameTimer& gt) 
{
	sceneManager->Update(gt);
	inputManager->ReleaseEvent();
}

void FrameworkApp::OnMouseDown(WPARAM btnState, int x, int y) 
{
	lastMousePos.x = x;
	lastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void FrameworkApp::OnMouseUp(WPARAM btnState, int x, int y) 
{
	ReleaseCapture();
}

void FrameworkApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if (GetCapture() == mhMainWnd) {

		if ((btnState & MK_LBUTTON) != 0) {
			float axisX{ static_cast<float>(x - lastMousePos.x) };
			axisX = std::clamp(axisX, -1.0f, 1.0f);
			inputManager->PushAxisEvent(static_cast<unsigned char>(btnState), axisX);
		}
		else if ((btnState & MK_RBUTTON) != 0) {
			float axisY{ XMConvertToRadians(0.25f * static_cast<float>(y - lastMousePos.y)) };
			axisY = std::clamp(axisY, -1.0f, 1.0f);
			inputManager->PushAxisEvent(static_cast<unsigned char>(btnState), axisY);
		}

		lastMousePos.x = x;
		lastMousePos.y = y;
	}
}

void FrameworkApp::OnKeyboardDown(unsigned char key, unsigned char state)
{
	inputManager->PushEvent(key, state);
}

void FrameworkApp::OnKeyboardUp(unsigned char key, unsigned char state)
{
	inputManager->PushEvent(key, state);
}

void FrameworkApp::Draw()
{
	FailedAssert(mDirectCmdListAlloc->Reset());
	FailedAssert(mCommandList->Reset(mDirectCmdListAlloc.Get(), GameplayStatics::GetPSO("Opaque")));

	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	const auto&& prTransition{ CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET) };

	mCommandList->ResourceBarrier(1, &prTransition);

	mCommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::BlanchedAlmond, 0, nullptr);
	mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	
	const auto&& curBackBufferView{ CurrentBackBufferView() };
	const auto&& curDepthStencilView{ DepthStencilView() };

	mCommandList->OMSetRenderTargets(1, &curBackBufferView, true, &curDepthStencilView);

	sceneManager->Draw(mCommandList.Get());

	// 인덱스 크기, 그릴갯수, 인덱스 시작점, 정점 시작점, 인스턴싱

	const auto&& rpTransition{ CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT) };

	mCommandList->ResourceBarrier(1, &rpTransition);

	FailedAssert(mCommandList->Close());

	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	mSwapChain->Present(0, 0);
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

	FlushCommandQueue();
}