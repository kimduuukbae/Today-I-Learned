#include "stdafx.h"
#include "BoxApp.h"
#include "DDSTextureLoader.h"

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace D3D;
BoxApp::BoxApp(HINSTANCE hInstance) : D3DApp{ hInstance } {}

BoxApp::~BoxApp() {
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
	ID3D12CommandList* cmdsLists[]{ mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	FlushCommandQueue();
	geoMetry->DisposeUploaders();
	return true;
}

void BoxApp::OnResize() {
	D3DApp::OnResize();

	XMMATRIX p{ XMMatrixPerspectiveFovLH(0.25f * XM_PI, AspectRatio(), 1.0f, 1000.0f) };
	XMStoreFloat4x4(&mProj, p);
}

void BoxApp::Update(const GameTimer& gt) {
	// �� ��ǥ���� ��ī��Ʈ ��ǥ��� ���� (���� ��ǥ��)
	float x = radius * sinf(phi) * cosf(theta);
	float z = radius * sinf(phi) * sinf(theta);
	float y = radius * cosf(phi);

	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX view2 = XMMatrixLookAtLH(pos, target, up);
	XMStoreFloat4x4(&mView, view2);
	mEyePos = { x, y, z };

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
	mMainPassCB.AmbientLight = { 0.0f, 0.0f, 0.0f, 1.0f };
	mMainPassCB.Lights[0].Direction = { 0.57735f, -0.57735f, 0.57735f };
	mMainPassCB.Lights[0].Strength = {1.0f, 1.0f, 1.0f };
	mMainPassCB.Lights[1].Direction = { -0.57735f, -0.57735f, 0.57735f };
	mMainPassCB.Lights[1].Strength = { 0.15f, 0.15f, 0.15f };
	mMainPassCB.Lights[2].Direction = { 0.0f, -0.707f, -0.707f };
	mMainPassCB.Lights[2].Strength = { 0.15f, 0.15f, 0.15f };

	passCB->CopyData(0, mMainPassCB);

	for (auto& it : mObjects) {
		Object& obj{ it };
		ObjectConstants objConst;
		XMStoreFloat4x4(&objConst.texMat, XMMatrixTranspose(XMLoadFloat4x4(&obj.objConst.texMat)));
		XMStoreFloat4x4(&objConst.world, XMMatrixTranspose(XMLoadFloat4x4(&obj.objConst.world)));
		objectCB->CopyData(it.CBIndex, objConst);
	}
}

void BoxApp::OnMouseDown(WPARAM btnState, int x, int y) {
	lastMousePos.x = x;
	lastMousePos.y = y;

	SetCapture(mhMainWnd);
}

void BoxApp::OnMouseUp(WPARAM btnState, int x, int y) {
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
	FailedAssert(mCommandList->Reset(mDirectCmdListAlloc.Get(), mPSO["opaque"].Get()));

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

	ID3D12DescriptorHeap* descriptorHeaps[] = { srvHeap.Get() };
	mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	// ���� Ÿ���� ���� �ϳ��� �� �� ����!

	mCommandList->SetGraphicsRootSignature(rootSignature.Get());

	mCommandList->IASetVertexBuffers(0, 1, &geoMetry->VertexBufferView("Box"));
	mCommandList->IASetIndexBuffer(&geoMetry->IndexBufferView("Box"));
	mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	mCommandList->SetGraphicsRootConstantBufferView(0, objectCB->GetResource()->GetGPUVirtualAddress());
	mCommandList->SetGraphicsRootConstantBufferView(1, matCB->GetResource()->GetGPUVirtualAddress());
	mCommandList->SetGraphicsRootConstantBufferView(2, passCB->GetResource()->GetGPUVirtualAddress());
	mCommandList->SetGraphicsRootDescriptorTable(3, srvHeap->GetGPUDescriptorHandleForHeapStart());

	mCommandList->DrawIndexedInstanced(geoMetry->Meshs["Box"].IndexCount, 1, 0, 0, 0);

	// �ε��� ũ��, �׸�����, �ε��� ������, ���� ������, �ν��Ͻ�

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
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc{};
	srvHeapDesc.NumDescriptors = 2;
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvHeapDesc.NodeMask = 0;
	FailedAssert(md3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&srvHeap)));
}

void BoxApp::BuildConstantBuffers() {
	objectCB = std::make_unique<D3D::UploadBuffer<ObjectConstants>>(md3dDevice.Get(), 1, true);
	matCB = std::make_unique<D3D::UploadBuffer<MaterialConstants>>(md3dDevice.Get(), 1, true);
	passCB = std::make_unique<D3D::UploadBuffer<PassConstants>>(md3dDevice.Get(), 1, true);

	//Texture
	Texture brickT{ CreateTexture(md3dDevice.Get(), mCommandList.Get(), L"Textures/bricks.dds", "BricksD") };
	mTextures["BricksD"] = std::make_unique<Texture>(brickT);

	Texture brickN{ CreateTexture(md3dDevice.Get(), mCommandList.Get(), L"Textures/bricks_nmap.dds", "BricksN") };
	mTextures["BricksN"] = std::make_unique<Texture>(brickN);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Texture2D.MipLevels = -1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.PlaneSlice = 0;
	srvDesc.Texture2D.ResourceMinLODClamp = 0;
	srvDesc.Format = mTextures["BricksD"]->Resource->GetDesc().Format;

	D3D12_CPU_DESCRIPTOR_HANDLE srvCPUHandle{ srvHeap->GetCPUDescriptorHandleForHeapStart() };

	md3dDevice->CreateShaderResourceView(mTextures["BricksD"]->Resource.Get(), &srvDesc,
		GetDescriptorHandleOffset(srvCPUHandle, 0, mCbvSrvUavDescriptorSize));

	srvDesc.Format = mTextures["BricksN"]->Resource->GetDesc().Format;
	md3dDevice->CreateShaderResourceView(mTextures["BricksN"]->Resource.Get(), &srvDesc,
		GetDescriptorHandleOffset(srvCPUHandle, 1, mCbvSrvUavDescriptorSize));

	Material brickM{};
	brickM.DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 0.6f };
	brickM.DiffuseSrvHeapIndex = 0;
	brickM.Roughness = 0.25f;
	brickM.MatCBIndex = 0;

	mMaterials["BrickM"] = std::make_unique<Material>(brickM);
}

void BoxApp::BuildRootSignature() {
	// �Ϲ�������, ���̴� ���α׷��� Ư�� ���ҽ���(��� ����, �ؽ���, ���÷� ��) �� 
	// �Էµȴٰ� ����Ѵ�. ��Ʈ �ñ״��Ĵ� ���̴� ���α׷��� ����ϴ� ���ҽ�����
	// �����Ѵ�. ���̴� ���α׷��� ���������� �ϳ��� �Լ��̰� ���̴��� �ԷµǴ�
	// �ڿ����� �Լ��� �Ű������� �ش��ϹǷ�, ��Ʈ �ñ״��Ĵ� �� �Լ� �ñ״��ĸ� �����ϴ�
	// �����̶� �� �� �ִ�.

	// ��Ʈ �Ķ���ʹ� ��Ʈ ���, ��Ʈ ������, ������ ���̺� �� ���� �ϳ��̴�.
	D3D12_ROOT_PARAMETER rootParameter[4]{};

	// ���̴� ���ҽ� �並 '�ϳ�' ��� ������ ���̺��� �����Ѵ�.
	D3D12_DESCRIPTOR_RANGE srvTable{};
	srvTable.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvTable.NumDescriptors = 2;
	srvTable.BaseShaderRegister = 0;

	rootParameter[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	// world Matrix
	rootParameter[0].Descriptor.RegisterSpace = 0;
	rootParameter[0].Descriptor.ShaderRegister = 0;
	rootParameter[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;

	rootParameter[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;	// 
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

	// ��Ʈ�ñ״��� ������
	D3D12_ROOT_SIGNATURE_DESC rootSigDesc{};
	rootSigDesc.NumParameters = 4;
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

	mShaders["OpaqueVS"] = D3D::CompileShader(L"Shaders\\Default.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["OpaquePS"] = D3D::CompileShader(L"Shaders\\Default.hlsl", nullptr, "PS", "ps_5_1");
	mShaders["TransparencyPS"] = D3D::CompileShader(L"Shaders\\Default.hlsl", nullptr, "TransparencyPS", "ps_5_1");
	mShaders["SkyboxVS"] = D3D::CompileShader(L"Shaders\\Skybox.hlsl", nullptr, "VS", "vs_5_1");
	mShaders["SkyboxPS"] = D3D::CompileShader(L"Shaders\\Skybox.hlsl", nullptr, "PS", "ps_5_1");

	mInputLayouts["Default"] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
	};
	mInputLayouts["Skybox"] = {
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
	};


	// Semantic �̸�, Semantic �ε���, Ÿ��, ����, ������
	// ���� Ÿ��, �ν��Ͻ� ����
}

void BoxApp::BuildBoxGeometry() {

	geoMetry = std::make_unique<MeshGeometry>();
	geoMetry->Name = "BoxGeo";

	MeshData boxData{ CreateBox(2.0f, 2.0f, 2.0f, md3dDevice.Get(), mCommandList.Get()) };
	geoMetry->Meshs["Box"] = boxData;

	MeshData sphereData{ CreateSphere(0.5f, 20, 20, md3dDevice.Get(), mCommandList.Get()) };
	geoMetry->Meshs["Sphere"] = sphereData;

	mObjects.reserve(objectCB->getElementNum());

	ObjectConstants box1;
	box1.world._43 = -2.0f;
	box1.texMat._41 = 1.0f;

	mObjects.emplace_back(Object{ box1, 0 });
}

void BoxApp::BuildPSO() {
	//�����Ͷ�����
	D3D12_RASTERIZER_DESC rasDesc{};
	rasDesc.FillMode = D3D12_FILL_MODE_SOLID;
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
	blendDesc.AlphaToCoverageEnable = false;	//true�� �����ϸ� ����-������ ��ȯ Ȱ��ȭ (�Ĺ� �����̳� â�쿡 ������ ��Ƽ���ø� ���)
	blendDesc.IndependentBlendEnable = false;	//true�� �����ϸ� �� RT���� Blend�� �ٸ� ������� �� �� �ִ�. false�� 0��°�� ����
	blendDesc.RenderTarget[0].BlendEnable = false;	// blend Ȱ��ȭ
	blendDesc.RenderTarget[0].LogicOpEnable = false; // �� ������ blend Ȱ��ȭ ( blend�� ���� �� �� ���� )
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE; //blend Factor src
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO; //blend factor dst
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD; // RGB ȥ�� ������
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE; // blend factor alpha src
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO; // blend factor alpha dst
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD; // ���� ȥ�� ������
	blendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP; // �� ������ ȥ��
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL; // �ĸ� ���� ���� ä�� ����

	D3D12_DEPTH_STENCIL_DESC dsDesc{};
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL; // ���� ���� ���� Ȱ��ȭ
	dsDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS; // �����ֵ鸸
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = 0; // ���ٽ� ���� �ڵ��� StencilReadMask�� �ش�
	// if(StencilRef & StencilReadMask ? Value & StencilReadMask )
	dsDesc.StencilWriteMask = 0; // Ư�� ��Ʈ ���� ��ϵ��� �ʰ� �ϴ� ��Ʈ����ũ 0�̸� ���X
	// ���� �ﰢ���� ���� ���ٽ� ���� ���� ���
	dsDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP; // ���ٽ��� ����� ���� ���н� ���� �ϴ� ��� ����
	dsDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP; // �������н�, ���� �ϴ� ��� ����
	dsDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP; // ���ٽ�, ���� ����� ���� �ϴ� ��� ����
	dsDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER; // ���ٽ� ���� �� �Լ�
	// �ĸ� �ﰢ���� ���� ���ٽ� ���� ���� ���
	dsDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	psoDesc.InputLayout = { mInputLayouts["Default"].data(), static_cast<UINT>(mInputLayouts["Default"].size()) };
	psoDesc.pRootSignature = rootSignature.Get();
	psoDesc.VS = { reinterpret_cast<BYTE*>(mShaders["OpaqueVS"]->GetBufferPointer()), mShaders["OpaqueVS"]->GetBufferSize() };
	psoDesc.PS = { reinterpret_cast<BYTE*>(mShaders["OpaquePS"]->GetBufferPointer()), mShaders["OpaquePS"]->GetBufferSize() };
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
	FailedAssert(md3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSO["opaque"])));

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

	D3D12_GRAPHICS_PIPELINE_STATE_DESC transDesc{};
	transDesc.BlendState = blendDesc;
	transDesc.DepthStencilState = dsDesc;
	transDesc.DSVFormat = mDepthStencilFormat;
	transDesc.InputLayout = { mInputLayouts["Default"].data(), static_cast<UINT>(mInputLayouts["Default"].size()) };
	transDesc.NumRenderTargets = 1;
	transDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	transDesc.pRootSignature = rootSignature.Get();
	transDesc.VS = { reinterpret_cast<BYTE*>(mShaders["OpaqueVS"]->GetBufferPointer()), mShaders["OpaqueVS"]->GetBufferSize() };
	transDesc.PS = { reinterpret_cast<BYTE*>(mShaders["TransparencyPS"]->GetBufferPointer()), mShaders["TransparencyPS"]->GetBufferSize() };
	transDesc.RasterizerState = rasDesc;
	transDesc.RTVFormats[0] = mBackBufferFormat;
	transDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	transDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	transDesc.SampleMask = UINT_MAX;

	md3dDevice->CreateGraphicsPipelineState(&transDesc, IID_PPV_ARGS(&mPSO["transparency"]));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC skyBoxDesc{ psoDesc };
	skyBoxDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	skyBoxDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	skyBoxDesc.pRootSignature = rootSignature.Get();
	skyBoxDesc.VS = { reinterpret_cast<BYTE*>(mShaders["SkyboxVS"]->GetBufferPointer()), mShaders["SkyboxVS"]->GetBufferSize() };
	skyBoxDesc.PS = { reinterpret_cast<BYTE*>(mShaders["SkyboxPS"]->GetBufferPointer()), mShaders["SkyboxPS"]->GetBufferSize() };

	md3dDevice->CreateGraphicsPipelineState(&skyBoxDesc, IID_PPV_ARGS(&mPSO["Skybox"]));
}