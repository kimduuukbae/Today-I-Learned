#include "stdafx.h"
#include "FrameworkApp.h"
#include "SceneManager.h"

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace D3D;
using namespace std;

FrameworkApp::FrameworkApp(HINSTANCE hInstance) : D3DApp{ hInstance } {}

FrameworkApp::~FrameworkApp() {
}

bool FrameworkApp::Initialize() {
	if (!D3DApp::Initialize())
		return false;

	sceneManager = SceneManager::GetInstance();

	sceneManager->Init();

	FailedAssert(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	FailedAssert(mCommandList->Close());
	ID3D12CommandList* cmdsLists[]{ mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	FlushCommandQueue();
	return true;
}

void FrameworkApp::OnResize() 
{
	D3DApp::OnResize();
}

void FrameworkApp::Update(const GameTimer& gt) 
{
	sceneManager->Update(gt);
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
	
}

void FrameworkApp::Draw()
{
	FailedAssert(mDirectCmdListAlloc->Reset());
	FailedAssert(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Transition.pResource = CurrentBackBuffer();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	mCommandList->ResourceBarrier(1, &barrier);

	mCommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::Brown, 0, nullptr);
	mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

	//ID3D12DescriptorHeap* descriptorHeaps[] = { srvHeap.Get() };
	//mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	// 같은 타입의 힙은 하나만 달 수 있음!

	sceneManager->Draw();

	// 인덱스 크기, 그릴갯수, 인덱스 시작점, 정점 시작점, 인스턴싱

	barrier.Transition.pResource = CurrentBackBuffer();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	mCommandList->ResourceBarrier(1, &barrier);

	FailedAssert(mCommandList->Close());

	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	mSwapChain->Present(0, 0);
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

	FlushCommandQueue();
}