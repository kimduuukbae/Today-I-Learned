#include "stdafx.h"
#include "BoxApp.h"

using namespace DirectX;
using namespace Microsoft::WRL;

BoxApp::BoxApp(HINSTANCE hInstance) : D3DApp{ hInstance } {}

BoxApp::~BoxApp(){
}

bool BoxApp::Initialize() {
	if (!D3DApp::Initialize())
		return false;

	FailedAssert(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	BuildDescriptorHeaps();
	BuildConstantBuffers();
	BuildRootSignature();
	BuildShadersAndInputLayout();
	BuildBoxGeometry();
	BuildPSO();

	FailedAssert(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	FlushCommandQueue();
	BoxGeometry->DisposeUploaders();
	return true;
}

void BoxApp::OnResize(){
	D3DApp::OnResize();

	XMMATRIX p{ XMMatrixPerspectiveFovLH(0.25f * XM_PI, AspectRatio(), 1.0f, 1000.0f) };
	XMStoreFloat4x4(&projMatrix, p);
}

void BoxApp::Update(const GameTimer& gt){
	// �� ��ǥ���� ��ī��Ʈ ��ǥ��� ���� (���� ��ǥ��)
	float x = radius * sinf(phi) * cosf(theta);
	float z = radius * sinf(phi) * sinf(theta);
	float y = radius * cosf(phi);

	// Build the view matrix.
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&viewMatrix, view);
	worldMatrix._42 = -1.0f;
	XMMATRIX world = XMLoadFloat4x4(&worldMatrix);
	XMMATRIX proj = XMLoadFloat4x4(&projMatrix);
	XMMATRIX worldViewProj = world * view * proj;
	// Update the constant buffer with the latest worldViewProj matrix.
	ObjectConstants objConstants;
	XMStoreFloat4x4(&objConstants.worldViewProjMat, XMMatrixTranspose(worldViewProj));
	objectCB->CopyData(0, objConstants);
}

void BoxApp::OnMouseDown(WPARAM btnState, int x, int y){
	lastMousePos.x = x;
	lastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void BoxApp::OnMouseUp(WPARAM btnState, int x, int y){
	ReleaseCapture();
}

void BoxApp::OnMouseMove(WPARAM btnState, int x, int y)
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - lastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - lastMousePos.y));

		// Update angles based on input to orbit camera around box.
		theta += dx;
		phi += dy;

		// Restrict the angle mPhi.
		phi = std::clamp(phi, 0.1f, XM_PI - 0.1f);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		// Make each pixel correspond to 0.005 unit in the scene.
		float dx = 0.005f * static_cast<float>(x - lastMousePos.x);
		float dy = 0.005f * static_cast<float>(y - lastMousePos.y);

		// Update the camera radius based on input.
		radius += dx - dy;

		// Restrict the radius.
		radius = std::clamp(radius, 3.0f, 15.0f);
	}

	lastMousePos.x = x;
	lastMousePos.y = y;
}

void BoxApp::Draw(const GameTimer& gt)
{
	// Reuse the memory associated with command recording.
	// We can only reset when the associated command lists have finished execution on the GPU.
	FailedAssert(mDirectCmdListAlloc->Reset());

	// A command list can be reset after it has been added to the command queue via ExecuteCommandList.
	// Reusing the command list reuses memory.
	FailedAssert(mCommandList->Reset(mDirectCmdListAlloc.Get(), PSO.Get()));

	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	// Indicate a state transition on the resource usage.
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Transition.pResource = CurrentBackBuffer();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	mCommandList->ResourceBarrier(1, &barrier);

	// Clear the back buffer and depth buffer.
	mCommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
	mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Specify the buffers we are going to render to.
	mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

	ID3D12DescriptorHeap* descriptorHeaps[] = { cbvHeap.Get() };
	mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	mCommandList->SetGraphicsRootSignature(rootSignature.Get());

	mCommandList->IASetVertexBuffers(0, 1, &BoxGeometry->VertexBufferView());
	mCommandList->IASetIndexBuffer(&BoxGeometry->IndexBufferView());
	mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	D3D12_GPU_DESCRIPTOR_HANDLE address{ cbvHeap->GetGPUDescriptorHandleForHeapStart() };

	mCommandList->SetGraphicsRootDescriptorTable(0, address);

	mCommandList->DrawIndexedInstanced(
		BoxGeometry->DrawArgs["box"].IndexCount,
		1, 0, 0, 0);

	// �ε��� ũ��, �׸�����, �ε��� ������, ���� ������, �ν��Ͻ�

	// Indicate a state transition on the resource usage.
	barrier.Transition.pResource = CurrentBackBuffer();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	mCommandList->ResourceBarrier(1, &barrier);

	// Done recording commands.
	FailedAssert(mCommandList->Close());

	// Add the command list to the queue for execution.
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// swap the back and front buffers
	FailedAssert(mSwapChain->Present(0, 0));
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

	// Wait until frame commands are complete.  This waiting is inefficient and is
	// done for simplicity.  Later we will show how to organize our rendering code
	// so we do not have to wait per frame.
	FlushCommandQueue();
}

void BoxApp::BuildDescriptorHeaps() {
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc{};
	cbvHeapDesc.NumDescriptors = 1;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	FailedAssert(md3dDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&cbvHeap)));
}

void BoxApp::BuildConstantBuffers() {
	objectCB = std::make_unique<D3D::UploadBuffer<ObjectConstants>>(md3dDevice.Get(), 1, true);

	constexpr UINT CBByteSize{ D3D::ConstantBufferByteSize(sizeof(ObjectConstants)) };

	D3D12_GPU_VIRTUAL_ADDRESS cbAddress{ objectCB->GetResource()->GetGPUVirtualAddress() };

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = CBByteSize;
	md3dDevice->CreateConstantBufferView(&cbvDesc, cbvHeap->GetCPUDescriptorHandleForHeapStart());
}

void BoxApp::BuildRootSignature() {
	// �Ϲ�������, ���̴� ���α׷��� Ư�� ���ҽ���(��� ����, �ؽ���, ���÷� ��) �� 
	// �Էµȴٰ� ����Ѵ�. ��Ʈ �ñ״��Ĵ� ���̴� ���α׷��� ����ϴ� ���ҽ�����
	// �����Ѵ�. ���̴� ���α׷��� ���������� �ϳ��� �Լ��̰� ���̴��� �ԷµǴ�
	// �ڿ����� �Լ��� �Ű������� �ش��ϹǷ�, ��Ʈ �ñ״��Ĵ� �� �Լ� �ñ״��ĸ� �����ϴ�
	// �����̶� �� �� �ִ�.

	// ��Ʈ �Ķ���ʹ� ��Ʈ ���, ��Ʈ ������, ������ ���̺� �� ���� �ϳ��̴�.
	D3D12_ROOT_PARAMETER rootParameter[1]{};

	// ��� ���� �並 '�ϳ�' ��� ������ ���̺��� �����Ѵ�.
	D3D12_DESCRIPTOR_RANGE cbvTable1{};
	cbvTable1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	cbvTable1.NumDescriptors = 1;
	cbvTable1.BaseShaderRegister = 0;		// 0�� ��������

	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[0].DescriptorTable.NumDescriptorRanges = 1;
	rootParameter[0].DescriptorTable.pDescriptorRanges = &cbvTable1;

	// ��Ʈ�ñ״��� ������
	D3D12_ROOT_SIGNATURE_DESC rootSigDesc{};
	rootSigDesc.NumParameters = 1;
	rootSigDesc.pParameters = rootParameter;
	rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// ��Ʈ�ñ״��� ����ȭ ( ��� ���� �ϳ��� ������ ������ Range�� ����Ű�� )
	// ���� �ϳ��� �̷���� ��Ʈ �ñ״��ĸ� ����
	ComPtr<ID3DBlob> serializedRootSig{ nullptr };
	ComPtr<ID3DBlob> errorBlob{ nullptr };
	HRESULT hr{ D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf()) };

	if (errorBlob)
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());

	FailedAssert(hr);

	// ��Ʈ�ñ״��� ����
	md3dDevice->CreateRootSignature(0, serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
}

void BoxApp::BuildShadersAndInputLayout() {
	VSByteCode = D3D::CompileShader(L"Shaders\\color.hlsl", nullptr, "VS", "vs_5_0");
	PSByteCode = D3D::CompileShader(L"Shaders\\color.hlsl", nullptr, "PS", "ps_5_0");

	inputLayout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};

	// Semantic �̸�, Semantic �ε���, Ÿ��, ����, ������
	// ���� Ÿ��, �ν��Ͻ� ����
}

void BoxApp::BuildBoxGeometry() {
	const auto vertexSet{ loadVertexFromFile("Models/skull.txt") };

	const UINT vbByteSize = (UINT)vertexSet.first.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)vertexSet.second.size() * sizeof(std::uint32_t);

	BoxGeometry = std::make_unique<MeshGeometry>();
	BoxGeometry->Name = "BoxGeo";

	// ����Ʈ ���� ����
	BoxGeometry->VertexBufferGPU = D3D::CreateDefaultBuffer(md3dDevice.Get(),
		mCommandList.Get(), vertexSet.first.data(), vbByteSize, BoxGeometry->VertexBufferUploader);
	BoxGeometry->IndexBufferGPU = D3D::CreateDefaultBuffer(md3dDevice.Get(),
		mCommandList.Get(), vertexSet.second.data(), ibByteSize, BoxGeometry->IndexBufferUploader);

	BoxGeometry->VertexByteStride = sizeof(Vertex);
	BoxGeometry->VertexBufferByteSize = vbByteSize;
	BoxGeometry->IndexFormat = DXGI_FORMAT_R32_UINT;
	BoxGeometry->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)vertexSet.second.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	BoxGeometry->DrawArgs["box"] = submesh;
}

void BoxApp::BuildPSO() {
	//�����Ͷ�����
	D3D12_RASTERIZER_DESC rasDesc{};
	rasDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
	rasDesc.CullMode = D3D12_CULL_MODE_BACK;
	rasDesc.FrontCounterClockwise = false; // ������ �ð���� ? false = �ð����
	rasDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS; // �ȼ��� ���̰��� ������ ���� ���̾ ��
	rasDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP; // �ȼ��� ���̰��� ������ �ִ� ��
	rasDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS; // �ȼ��� ���⿡ ���� ���̾ ���� ��
	rasDesc.DepthClipEnable = true; // ���̰��� ����� Ŭ����
	rasDesc.MultisampleEnable = false; // ���߻��ø�
	rasDesc.AntialiasedLineEnable = false; // ��Ƽ�ٸ����
	rasDesc.ForcedSampleCount = 0; // UAV �������� ����� ���� ����
	rasDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	// ������ �����Ͷ�����

	D3D12_BLEND_DESC blendDesc{};
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = false;
	blendDesc.RenderTarget[0].LogicOpEnable = false;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	D3D12_DEPTH_STENCIL_DESC dsDesc{};
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = 0;
	dsDesc.StencilWriteMask = 0;
	dsDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	dsDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.InputLayout = { inputLayout.data(), (UINT)inputLayout.size() };
	psoDesc.pRootSignature = rootSignature.Get();
	psoDesc.VS = { reinterpret_cast<BYTE*>(VSByteCode->GetBufferPointer()), VSByteCode->GetBufferSize() };
	psoDesc.PS = D3D12_SHADER_BYTECODE{ reinterpret_cast<BYTE*>(PSByteCode->GetBufferPointer()), PSByteCode->GetBufferSize() };
	psoDesc.RasterizerState = rasDesc;
	psoDesc.BlendState = blendDesc;
	psoDesc.DepthStencilState = dsDesc;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = mBackBufferFormat;
	psoDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	psoDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	psoDesc.DSVFormat = mDepthStencilFormat;
	FailedAssert(md3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&PSO)));
}