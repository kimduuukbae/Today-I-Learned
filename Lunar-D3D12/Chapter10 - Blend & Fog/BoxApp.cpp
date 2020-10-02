#include "stdafx.h"
#include "BoxApp.h"
#include "DDSTextureLoader.h"

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace D3D;
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
	XMStoreFloat4x4(&mProj, p);
}

void BoxApp::Update(const GameTimer& gt){
	// 구 좌표에서 데카르트 좌표계로 변경 (직각 좌표계)
	float x = radius * sinf(phi) * cosf(theta);
	float z = radius * sinf(phi) * sinf(theta);
	float y = radius * cosf(phi);

	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view2 = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, view2);
	mEyePos = { x, y, z };
	ObjectConstants obj{VectorMath::Identity4X4()};
	obj.worldViewProjMat._43 = -2.0f;
	obj.texMat._41 = 1.0f;
	
	XMMATRIX world{ XMLoadFloat4x4(&obj.worldViewProjMat) };
	XMStoreFloat4x4(&obj.worldViewProjMat, XMMatrixTranspose(world));
	XMMATRIX texMat{ XMLoadFloat4x4(&obj.texMat) };
	XMStoreFloat4x4(&obj.texMat, XMMatrixTranspose(texMat));
	objectCB->CopyData(0, obj);

	ObjectConstants obj2{ VectorMath::Identity4X4() };
	obj2.worldViewProjMat._43 = 0.0f;
	obj2.texMat._41 = 1.0f;
	XMMATRIX world2{ XMLoadFloat4x4(&obj2.worldViewProjMat) };
	XMStoreFloat4x4(&obj2.worldViewProjMat, XMMatrixTranspose(world2));
	XMMATRIX texMat2{ XMLoadFloat4x4(&obj2.texMat) };
	XMStoreFloat4x4(&obj2.texMat, XMMatrixTranspose(texMat2));

	objectCB->CopyData(1, obj2);

	MaterialConstants matConstants{};
	
	for (auto& e : mMaterials) {
		Material* mat = e.second.get();
		XMMATRIX matTransform{ XMLoadFloat4x4(&mat->MatTransform) };

		MaterialConstants matConstants{};
		matConstants.DiffuseAlbedo = mat->DiffuseAlbedo;
		matConstants.FresnelR0 = mat->FresnelR0;
		matConstants.Roughness = mat->Roughness;
		XMStoreFloat4x4(&matConstants.MatTransform, XMMatrixTranspose(matTransform));

		matCB->CopyData(mat->MatCBIndex, matConstants);
	}

	XMMATRIX view = XMLoadFloat4x4(&mView);
	XMMATRIX proj = XMLoadFloat4x4(&mProj);

	XMMATRIX viewProj = XMMatrixMultiply(view, proj);
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(view), view);
	XMMATRIX invProj = XMMatrixInverse(&XMMatrixDeterminant(proj), proj);
	XMMATRIX invViewProj = XMMatrixInverse(&XMMatrixDeterminant(viewProj), viewProj);

	XMStoreFloat4x4(&mMainPassCB.View, XMMatrixTranspose(view));
	XMStoreFloat4x4(&mMainPassCB.InvView, XMMatrixTranspose(invView));
	XMStoreFloat4x4(&mMainPassCB.Proj, XMMatrixTranspose(proj));
	XMStoreFloat4x4(&mMainPassCB.InvProj, XMMatrixTranspose(invProj));
	XMStoreFloat4x4(&mMainPassCB.ViewProj, XMMatrixTranspose(viewProj));
	XMStoreFloat4x4(&mMainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
	mMainPassCB.EyePosW = mEyePos;
	mMainPassCB.RenderTargetSize = XMFLOAT2((float)mClientWidth, (float)mClientHeight);
	mMainPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / mClientWidth, 1.0f / mClientHeight);
	mMainPassCB.NearZ = 1.0f;
	mMainPassCB.FarZ = 1000.0f;
	mMainPassCB.TotalTime = gt.TotalTime();
	mMainPassCB.DeltaTime = gt.DeltaTime();
	mMainPassCB.AmbientLight = { 0.5f, 0.5f, 0.7f, 1.0f };
	mMainPassCB.Lights[0].Direction = { 0.57735f, -0.57735f, 0.57735f };
	mMainPassCB.Lights[0].Strength = { 0.15f, 0.15f, 0.15f };
	mMainPassCB.Lights[1].Direction = { -0.57735f, -0.57735f, 0.57735f };
	mMainPassCB.Lights[1].Strength = { 0.15f, 0.15f, 0.15f };
	mMainPassCB.Lights[2].Direction = { 0.0f, -0.707f, -0.707f };
	mMainPassCB.Lights[2].Strength = { 0.15f, 0.15f, 0.15f };

	passCB->CopyData(0, mMainPassCB);
	Time t{};
	t.time = gt.TotalTime();
	timeCB->CopyData(0, t);
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
		float dx = XMConvertToRadians(0.25f * static_cast<float>(x - lastMousePos.x));
		float dy = XMConvertToRadians(0.25f * static_cast<float>(y - lastMousePos.y));

		theta += dx;
		phi += dy;

		phi = std::clamp(phi, 0.1f, XM_PI - 0.1f);
	}
	else if ((btnState & MK_RBUTTON) != 0)
	{
		float dx = 0.005f * static_cast<float>(x - lastMousePos.x);
		float dy = 0.005f * static_cast<float>(y - lastMousePos.y);

		radius += dx - dy;

		radius = std::clamp(radius, 3.0f, 150.0f);
	}

	lastMousePos.x = x;
	lastMousePos.y = y;
}

void BoxApp::Draw(const GameTimer& gt)
{
	FailedAssert(mDirectCmdListAlloc->Reset());
	FailedAssert(mCommandList->Reset(mDirectCmdListAlloc.Get(), opaquePSO.Get()));
//	mCommandList->SetPipelineState(opaquePSO.Get());
	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Transition.pResource = CurrentBackBuffer();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	mCommandList->ResourceBarrier(1, &barrier);

	mCommandList->ClearRenderTargetView(CurrentBackBufferView(), Colors::Black, 0, nullptr);
	mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

	ID3D12DescriptorHeap* descriptorHeaps[] = { cbvHeap.Get() };
	mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	mCommandList->SetGraphicsRootSignature(rootSignature.Get());

	mCommandList->IASetVertexBuffers(0, 1, &BoxGeometry->VertexBufferView());
	mCommandList->IASetIndexBuffer(&BoxGeometry->IndexBufferView());
	mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	
	D3D12_GPU_DESCRIPTOR_HANDLE cbv{ cbvHeap->GetGPUDescriptorHandleForHeapStart() };
	D3D12_GPU_DESCRIPTOR_HANDLE srv{ cbvHeap->GetGPUDescriptorHandleForHeapStart() };
	srv.ptr += (mCbvSrvUavDescriptorSize * 2);
	
	mCommandList->SetGraphicsRootDescriptorTable(0, cbv);	// cbv
	mCommandList->SetGraphicsRootDescriptorTable(3, srv);	// srv
	mCommandList->SetGraphicsRootConstantBufferView(1, matCB->GetResource()->GetGPUVirtualAddress());
	mCommandList->SetGraphicsRootConstantBufferView(2, passCB->GetResource()->GetGPUVirtualAddress());
	mCommandList->SetGraphicsRootConstantBufferView(4, timeCB->GetResource()->GetGPUVirtualAddress());
	mCommandList->DrawIndexedInstanced(
		BoxGeometry->DrawArgs["box"].IndexCount,
		1, 0, 0, 0);

	cbv.ptr += mCbvSrvUavDescriptorSize;
	srv.ptr += mCbvSrvUavDescriptorSize;
	mCommandList->SetGraphicsRootDescriptorTable(0, cbv);		// cbv
	mCommandList->SetGraphicsRootDescriptorTable(3, srv);	// srv
	mCommandList->SetGraphicsRootConstantBufferView(1, matCB->GetResource()->GetGPUVirtualAddress() + 256);

	mCommandList->DrawIndexedInstanced(
		BoxGeometry->DrawArgs["box"].IndexCount,
		1, 0, 0, 0);

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

void BoxApp::BuildDescriptorHeaps() {
	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc{};
	cbvHeapDesc.NumDescriptors = 4;
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvHeapDesc.NodeMask = 0;
	FailedAssert(md3dDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&cbvHeap)));
}

void BoxApp::BuildConstantBuffers() {
	objectCB = std::make_unique<D3D::UploadBuffer<ObjectConstants>>(md3dDevice.Get(), 2, true);
	matCB = std::make_unique<D3D::UploadBuffer<MaterialConstants>>(md3dDevice.Get(), 2, true);
	passCB = std::make_unique<D3D::UploadBuffer<PassConstants>>(md3dDevice.Get(), 1, true);
	timeCB = std::make_unique<D3D::UploadBuffer<Time>>(md3dDevice.Get(), 1, true);
	constexpr UINT CBByteSize{ D3D::ConstantBufferByteSize(sizeof(ObjectConstants)) };
	D3D12_GPU_VIRTUAL_ADDRESS cbAddress{ objectCB->GetResource()->GetGPUVirtualAddress() };

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
	cbvDesc.BufferLocation = cbAddress;
	cbvDesc.SizeInBytes = CBByteSize;

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc2{};
	cbvDesc2.BufferLocation = cbAddress + CBByteSize;
	cbvDesc2.SizeInBytes = CBByteSize;

	auto handle{ cbvHeap->GetCPUDescriptorHandleForHeapStart() };
	handle.ptr += mCbvSrvUavDescriptorSize;

	md3dDevice->CreateConstantBufferView(&cbvDesc, cbvHeap->GetCPUDescriptorHandleForHeapStart());
	md3dDevice->CreateConstantBufferView(&cbvDesc2, handle);

	//Texture
	Texture t{};
	t.FileName = L"Textures/WoodCrate01.dds";
	t.Name = "CrateTexture";
	CreateDDSTextureFromFile12(md3dDevice.Get(), mCommandList.Get(), t.FileName.c_str(), t.Resource, t.UploadHeap);
	mTextures["Wood"] = std::make_unique<Texture>(t);

	Texture w{};
	w.FileName = L"Textures/water1.dds";
	w.Name = "WaterTexture";
	CreateDDSTextureFromFile12(md3dDevice.Get(), mCommandList.Get(), w.FileName.c_str(), w.Resource, w.UploadHeap);
	mTextures["Water"] = std::make_unique<Texture>(w);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = 10;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.PlaneSlice = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc2{};
	srvDesc2.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc2.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc2.Texture2D.MipLevels = 9;
	srvDesc2.Texture2D.MostDetailedMip = 0;
	srvDesc2.Texture2D.PlaneSlice = 0;
	srvDesc2.Texture2D.ResourceMinLODClamp = 0;

	handle.ptr += mCbvSrvUavDescriptorSize;
	md3dDevice->CreateShaderResourceView(mTextures["Wood"].get()->Resource.Get(), &srvDesc, handle);
	handle.ptr += mCbvSrvUavDescriptorSize;
	md3dDevice->CreateShaderResourceView(mTextures["Water"].get()->Resource.Get(), &srvDesc2, handle);

	Material woodM{};
	woodM.DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 0.6f };
	woodM.DiffuseSrvHeapIndex = 0;
	woodM.Roughness = 0.25f;
	woodM.MatCBIndex = 0;

	Material waterM{};
	waterM.DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 0.4f };
	waterM.DiffuseSrvHeapIndex = 1;
	waterM.Roughness = 0.25f;
	waterM.MatCBIndex = 1;

	mMaterials["box"] = std::make_unique<Material>(woodM);
	mMaterials["water"] = std::make_unique<Material>(waterM);
}

void BoxApp::BuildRootSignature() {
	// 일반적으로, 셰이더 프로그램은 특정 리소스들(상수 버퍼, 텍스쳐, 샘플러 등) 이 
	// 입력된다고 기대한다. 루트 시그니쳐는 셰이더 프로그램이 기대하는 리소스들을
	// 정의한다. 셰이더 프로그램은 본질적으로 하나의 함수이고 셰이더에 입력되는
	// 자원들은 함수의 매개변수에 해당하므로, 루트 시그니쳐는 곧 함수 시그니쳐를 정의하는
	// 수단이라 할 수 있다.

	// 루트 파라미터는 루트 상수, 루트 서술자, 서술자 테이블 세 개중 하나이다.
	D3D12_ROOT_PARAMETER rootParameter[5]{};

	// 상수 버퍼 뷰를 '하나' 담는 서술자 테이블을 생성한다.
	D3D12_DESCRIPTOR_RANGE cbvTable1{};
	cbvTable1.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	cbvTable1.NumDescriptors = 1;
	cbvTable1.BaseShaderRegister = 0;		// 0번 레지스터

	D3D12_DESCRIPTOR_RANGE srvTable{};
	srvTable.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvTable.NumDescriptors = 1;
	srvTable.BaseShaderRegister = 0;

	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[0].DescriptorTable.NumDescriptorRanges = 1;
	rootParameter[0].DescriptorTable.pDescriptorRanges = &cbvTable1;

	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[1].Descriptor.RegisterSpace = 0;
	rootParameter[1].Descriptor.ShaderRegister = 1;
	rootParameter[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;

	rootParameter[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameter[2].Descriptor.RegisterSpace = 0;
	rootParameter[2].Descriptor.ShaderRegister = 2;
	rootParameter[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;

	rootParameter[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	rootParameter[3].DescriptorTable.NumDescriptorRanges = 1;
	rootParameter[3].DescriptorTable.pDescriptorRanges = &srvTable;

	rootParameter[4].Descriptor.RegisterSpace = 0;
	rootParameter[4].Descriptor.ShaderRegister = 3;
	rootParameter[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;


	// 루트시그니쳐 서술자
	D3D12_ROOT_SIGNATURE_DESC rootSigDesc{};
	rootSigDesc.NumParameters = 5;
	rootSigDesc.pParameters = rootParameter;
	rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
	
	D3D12_STATIC_SAMPLER_DESC ssDesc{};
	ssDesc.AddressU = ssDesc.AddressV = ssDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	ssDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
	ssDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	ssDesc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	ssDesc.MaxAnisotropy = 1;
	ssDesc.MaxLOD = D3D12_FLOAT32_MAX;
	ssDesc.ShaderRegister = 0;
	ssDesc.ShaderRegister = 0;
	ssDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	rootSigDesc.pStaticSamplers = &ssDesc;
	rootSigDesc.NumStaticSamplers = 1;
	// 루트시그니쳐 직렬화 ( 상수 버퍼 하나로 구성된 서술자 Range를 가리키는 )
	// 슬롯 하나로 이루어진 루트 시그니쳐를 생성
	ComPtr<ID3DBlob> serializedRootSig{ nullptr };
	ComPtr<ID3DBlob> errorBlob{ nullptr };
	HRESULT hr{ D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf()) };

	if (errorBlob)
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());

	FailedAssert(hr);

	// 루트시그니쳐 생성
	md3dDevice->CreateRootSignature(0, serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
}

void BoxApp::BuildShadersAndInputLayout() {
	VSByteCode = D3D::CompileShader(L"Shaders\\Default.hlsl", nullptr, "VS", "vs_5_1");
	PSByteCode = D3D::CompileShader(L"Shaders\\Default.hlsl", nullptr, "PS", "ps_5_1");
	OpaquePSByteCode = D3D::CompileShader(L"Shaders\\Default.hlsl", nullptr, "OpaquePS", "ps_5_1");
	inputLayout = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};

	// Semantic 이름, Semantic 인덱스, 타입, 슬롯, 오프셋
	// 슬롯 타입, 인스턴스 갯수
}

void BoxApp::BuildBoxGeometry() {

	BoxGeometry = std::make_unique<MeshGeometry>();
	BoxGeometry->Name = "BoxGeo";

	MeshData boxData{ CreateBox(2.0f, 2.0f, 2.0f, md3dDevice.Get(), mCommandList.Get()) };
	BoxGeometry->Data = boxData;

	SubmeshGeometry submesh;
	submesh.IndexCount = boxData.IndexCount;
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	BoxGeometry->DrawArgs["box"] = submesh;
}

void BoxApp::BuildPSO() {
	//래스터라이저
	D3D12_RASTERIZER_DESC rasDesc{};
	rasDesc.FillMode = D3D12_FILL_MODE_SOLID;
	rasDesc.CullMode = D3D12_CULL_MODE_BACK;
	rasDesc.FrontCounterClockwise = false; // 전면이 시계방향 ? false = 시계방향
	rasDesc.DepthBias = D3D12_DEFAULT_DEPTH_BIAS; // 픽셀의 깊이값에 더해질 깊이 바이어스 값
	rasDesc.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP; // 픽셀의 깊이값에 더해질 최대 값
	rasDesc.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS; // 픽셀의 기울기에 따라 바이어스 조절 값
	rasDesc.DepthClipEnable = true; // 깊이값을 사용한 클리핑
	rasDesc.MultisampleEnable = false; // 다중샘플링
	rasDesc.AntialiasedLineEnable = false; // 안티앨리어싱
	rasDesc.ForcedSampleCount = 0; // UAV 렌더링에 사용할 샘플 갯수
	rasDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	// 보수적 래스터라이저

	D3D12_BLEND_DESC blendDesc{};
	blendDesc.AlphaToCoverageEnable = false;	//true로 설정하면 알파-포괄도 변환 활성화 (식물 군엽이나 창살에 유용한 멀티샘플링 기법)
	blendDesc.IndependentBlendEnable = false;	//true로 설정하면 각 RT마다 Blend를 다른 방식으로 할 수 있다. false면 0번째꺼 따라씀
	blendDesc.RenderTarget[0].BlendEnable = false;	// blend 활성화
	blendDesc.RenderTarget[0].LogicOpEnable = false; // 논리 연산자 blend 활성화 ( blend와 같이 쓸 수 없음 )
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE; //blend Factor src
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO; //blend factor dst
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD; // RGB 혼합 연산자
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE; // blend factor alpha src
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO; // blend factor alpha dst
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD; // 알파 혼합 연산자
	blendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP; // 논리 연산자 혼합
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // 후면 버퍼 색상 채널 결정

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

	D3D12_RENDER_TARGET_BLEND_DESC transparencyBlendDesc{};
	
	transparencyBlendDesc.BlendEnable = true;
	transparencyBlendDesc.LogicOpEnable = false;
	transparencyBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;	
	transparencyBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	transparencyBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
	transparencyBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	transparencyBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
	transparencyBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	transparencyBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
	transparencyBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	blendDesc.RenderTarget[0] = transparencyBlendDesc;
	
	rasDesc.CullMode = D3D12_CULL_MODE_NONE;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaqueDesc{};
	opaqueDesc.BlendState = blendDesc;
	opaqueDesc.DepthStencilState = dsDesc;
	opaqueDesc.DSVFormat = mDepthStencilFormat;
	opaqueDesc.InputLayout = { inputLayout.data(), static_cast<UINT>(inputLayout.size()) };
	opaqueDesc.NumRenderTargets = 1;
	opaqueDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	opaqueDesc.pRootSignature = rootSignature.Get();
	opaqueDesc.PS = { reinterpret_cast<BYTE*>(OpaquePSByteCode->GetBufferPointer()), OpaquePSByteCode ->GetBufferSize()};
	opaqueDesc.RasterizerState = rasDesc;
	opaqueDesc.RTVFormats[0] = mBackBufferFormat;
	opaqueDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	opaqueDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	opaqueDesc.VS = { reinterpret_cast<BYTE*>(VSByteCode->GetBufferPointer()), VSByteCode->GetBufferSize()};
	opaqueDesc.SampleMask = UINT_MAX;

	md3dDevice->CreateGraphicsPipelineState(&opaqueDesc, IID_PPV_ARGS(&opaquePSO));
}