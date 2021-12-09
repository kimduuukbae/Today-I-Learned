#include "stdafx.h"
#include "ResourceManager.h"
#include "d3dx12.h"
#include "CameraComponent.h"
#include "Texture.h"
#include "GameplayStatics.h"
#include "LagCameraComponent.h"
#include "Object.h"
#include "BasicMesh.h"

using namespace Microsoft::WRL;
using namespace DirectX;

Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(const std::wstring_view& fileName, const D3D_SHADER_MACRO* defines, const std::string_view& entrypoint, const std::string_view& target)
{
	UINT compileFlags{};
#if defined(DEBUG) || defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ComPtr<ID3DBlob> byteCode{ nullptr };
	ComPtr<ID3DBlob> errorBlob{ nullptr };
	HRESULT hr = D3DCompileFromFile(fileName.data(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
		entrypoint.data(), target.data(), compileFlags, 0, &byteCode, &errorBlob);

	if (errorBlob)
		OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	FailedAssert(hr);

	return byteCode;
}

ResourceManager::~ResourceManager()
{
	passCB = nullptr;
}

void ResourceManager::Init()
{
	CreateShaderResourceView();
	CreateResources();
	CreateRootSignature();
	CreatePSO();
}

void ResourceManager::SetMainCamera(CameraComponent* camComp)
{
	mainCam = camComp;
}

ID3D12PipelineState* ResourceManager::GetPSO(const std::string& name)
{
	return psos[name].Get();
}

Texture* ResourceManager::LoadTexture(const std::filesystem::path& path)
{
	if (!std::filesystem::exists(path))
		return nullptr;

	std::filesystem::path prox{ std::filesystem::proximate(path) };
	std::wstring proxStr{ prox.generic_wstring() };

	if (textures.find(proxStr) == textures.end()) {
		textures[proxStr] = std::make_unique<Texture>(path.generic_wstring());
		textures[proxStr]->SetSrvIndex(textures.size() - 1);
	}

	return textures[proxStr].get();
}

Texture* ResourceManager::LoadTextureFromFile(FILE* file)
{
	char pstrTextureName[64] = { '\0' };

	BYTE nStrLength = 64;
	UINT nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, file);
	nReads = (UINT)::fread(pstrTextureName, sizeof(char), nStrLength, file);
	pstrTextureName[nStrLength] = '\0';

	bool bDuplicated = false;
	bool bLoaded = false;
	if (strcmp(pstrTextureName, "null"))
	{
		bLoaded = true;
		char pstrFilePath[64] = { '\0' };
		strcpy_s(pstrFilePath, 64, "Model/Textures/");

		bDuplicated = (pstrTextureName[0] == '@');
		strcpy_s(pstrFilePath + 15, 64 - 15, (bDuplicated) ? (pstrTextureName + 1) : pstrTextureName);
		strcpy_s(pstrFilePath + 15 + ((bDuplicated) ? (nStrLength - 1) : nStrLength), 64 - 15 - ((bDuplicated) ? (nStrLength - 1) : nStrLength), ".dds");

		return LoadTexture(pstrFilePath);
	}
	return nullptr;
}

void ResourceManager::BindingResource(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->SetPipelineState(psos["Debug"].Get());

	const auto&& barrier{ CD3DX12_RESOURCE_BARRIER::Transition(shadowMap->GetResource(),
	D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ) };
	cmdList->ResourceBarrier(1, &barrier);

	PassInfomation pass;

	XMMATRIX view{ mainCam->GetView() };
	XMMATRIX proj{ mainCam->GetProj() };
	XMVECTOR pos{ mainCam->GetPosition() };
	XMStoreFloat4x4(&pass.viewMatrix, XMMatrixTranspose(view));
	XMStoreFloat4x4(&pass.projMatrix, XMMatrixTranspose(proj));
	pass.eyePosition = mainCam->GetPosition3f();
	XMStoreFloat4x4(&pass.viewProj, XMMatrixTranspose(XMMatrixMultiply(view, proj)));
	pass.shadowTransform = st;
	pass.totalTime = GameplayStatics::GetTotalTime();

	pass.light[0].direction = { -0.57735f, -0.57735f, 0.57735f };
	pass.light[0].strength = { 2.0f, 2.0f, 2.0f };
	pass.ambient = { 0.2f, 0.2f , 0.2f, 0.2f};
	
	passCB->CopyData(pass);
	cmdList->SetGraphicsRootConstantBufferView(1, passCB->GetResource()->GetGPUVirtualAddress());
	cmdList->SetGraphicsRootDescriptorTable(3, shadowMap->srvHandle());

	MeshBase* b{ GameplayStatics::GetMesh("Quad") };
	b->BindingResource(cmdList);
	b->Draw(cmdList);

	cmdList->SetPipelineState(psos["Opaque"].Get());
}

void ResourceManager::ReleaseUploadBuffer()
{
	for (auto& it : textures)
		it.second->ReleaseUploadBuffer();
}

void ResourceManager::PreProcessing(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->RSSetViewports(1, &shadowMap->GetViewport());
	cmdList->RSSetScissorRects(1, &shadowMap->GetRect());

	const auto&& barrier{ CD3DX12_RESOURCE_BARRIER::Transition(shadowMap->GetResource(),
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE) };
	cmdList->ResourceBarrier(1, &barrier);
	
	D3D12_CPU_DESCRIPTOR_HANDLE depthHandle{ shadowMap->depthHandle() };

	cmdList->ClearDepthStencilView(depthHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		1.0f, 0, 0, nullptr);

	// 널 렌더 타겟을 사용하면 깊이 버퍼에만 작성함, GPU가 최적화 할 수 있음
	// 널 렌더 타겟 == color write를 끄는것과 똑같음

	cmdList->OMSetRenderTargets(0, nullptr, false, &depthHandle);

	cmdList->SetPipelineState(psos["Shadow"].Get());
	cmdList->SetGraphicsRootSignature(signature["Default"].Get());

	ID3D12DescriptorHeap* descriptorHeaps[] = { srvHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	PassInfomation pass;

	auto [x, y, z] = static_cast<LagCameraComponent*>(mainCam)->GetOwner()->GetTransform()->GetPosition();

	float radius{ 100.0f };
	XMVECTOR lightDir = XMVectorSet(-0.57735f, -0.57735f, 0.57735f, 0.0f);
	XMVECTOR lightPos = -2.0f * radius * lightDir;
	XMFLOAT3 camPos{ x, y, z };
	XMVECTOR targetPos = XMLoadFloat3(&camPos);
	XMVECTOR lightUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX lightView = XMMatrixLookAtLH(lightPos, targetPos, lightUp);

	XMStoreFloat3(&pass.eyePosition, lightPos);
	
	XMFLOAT3 sphereCenterLS;
	XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, lightView));

	float l = sphereCenterLS.x - radius;
	float b = sphereCenterLS.y - radius;
	float n = sphereCenterLS.z - radius;
	float r = sphereCenterLS.x + radius;
	float t = sphereCenterLS.y + radius;
	float f = sphereCenterLS.z + radius;

	XMMATRIX lightProj = XMMatrixOrthographicOffCenterLH(l, r, b, t, n, f);

	// NDC 공간에서 Texture공간으로 이동하는 Matrix
	XMMATRIX T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	XMMATRIX S = lightView * lightProj * T;
	XMStoreFloat4x4(&pass.viewMatrix, XMMatrixTranspose(lightView));
	XMStoreFloat4x4(&pass.projMatrix, XMMatrixTranspose(lightProj));
	XMStoreFloat4x4(&pass.viewProj, XMMatrixTranspose(lightView * lightProj));
	XMStoreFloat4x4(&st, XMMatrixTranspose(S));
	shadowCB->CopyData(pass);
	cmdList->SetGraphicsRootConstantBufferView(1, shadowCB->GetResource()->GetGPUVirtualAddress());
}

void ResourceManager::CreateRootSignature()
{
	ID3D12Device* device{ D3DApp::GetApp()->GetDevice() };
	D3D12_ROOT_PARAMETER param[4]{};

	param[0].Descriptor.ShaderRegister = 0;
	param[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	// WORLD
	param[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	param[1].Descriptor.ShaderRegister = 1;
	param[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	// Pass
	param[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_DESCRIPTOR_RANGE srvRange{};
	srvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvRange.NumDescriptors = 2;

	D3D12_DESCRIPTOR_RANGE smRange{};
	smRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	smRange.NumDescriptors = 1;
	smRange.BaseShaderRegister = 2;

	param[2].DescriptorTable.NumDescriptorRanges = 1;
	param[2].DescriptorTable.pDescriptorRanges = &srvRange;
	param[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	param[3].DescriptorTable.NumDescriptorRanges = 1;
	param[3].DescriptorTable.pDescriptorRanges = &smRange;
	param[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_STATIC_SAMPLER_DESC ssDesc{};

	const std::array<CD3DX12_STATIC_SAMPLER_DESC, 7> ssamples{
		CD3DX12_STATIC_SAMPLER_DESC(
			0, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_WRAP), // addressW

		CD3DX12_STATIC_SAMPLER_DESC(
			1, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP), // addressW

		CD3DX12_STATIC_SAMPLER_DESC(
			2, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_WRAP), // addressW

		CD3DX12_STATIC_SAMPLER_DESC(
			3, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP), // addressW

		CD3DX12_STATIC_SAMPLER_DESC(
			4, // shaderRegister
			D3D12_FILTER_ANISOTROPIC, // filter
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
			0.0f,                             // mipLODBias
			8),                               // maxAnisotropy

		CD3DX12_STATIC_SAMPLER_DESC(
			5, // shaderRegister
			D3D12_FILTER_ANISOTROPIC, // filter
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
			0.0f,                              // mipLODBias
			8),                                // maxAnisotropy

		CD3DX12_STATIC_SAMPLER_DESC(
		6, // shaderRegister
		D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressW
		0.0f,                               // mipLODBias
		16,                                 // maxAnisotropy
		D3D12_COMPARISON_FUNC_LESS_EQUAL,
		D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK)
	};

	D3D12_ROOT_SIGNATURE_DESC desc{};
	desc.pParameters = param;
	desc.NumParameters = 4;
	desc.NumStaticSamplers = static_cast<uint32_t>(ssamples.size());
	desc.pStaticSamplers = ssamples.data();
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ComPtr<ID3DBlob> serializeBlob{}, errorBlob{};
	D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, serializeBlob.GetAddressOf(), errorBlob.GetAddressOf());
	device->CreateRootSignature(0, serializeBlob->GetBufferPointer(), serializeBlob->GetBufferSize()
		, IID_PPV_ARGS(&signature["Default"]));

	serializeBlob->Release();
}

void ResourceManager::CreatePSO()
{
	D3DApp* app{ D3DApp::GetApp() };

	ComPtr<ID3DBlob> defaultVS{ CompileShader(L"Shaders\\Default.hlsl", nullptr, "VS", "vs_5_1") };
	ComPtr<ID3DBlob> defaultPS{ CompileShader(L"Shaders\\Default.hlsl", nullptr, "PS", "ps_5_1") };

	ComPtr<ID3DBlob> shadowVS{ CompileShader(L"Shaders\\Shadows.hlsl", nullptr, "VS", "vs_5_1") };
	ComPtr<ID3DBlob> shadowPS{ CompileShader(L"Shaders\\Shadows.hlsl", nullptr, "PS", "ps_5_1") };

	ComPtr<ID3DBlob> blendVS{ CompileShader(L"Shaders\\Blend.hlsl", nullptr, "VS", "vs_5_1") };
	ComPtr<ID3DBlob> blendPS{ CompileShader(L"Shaders\\Blend.hlsl", nullptr, "PS", "ps_5_1") };
	
	ComPtr<ID3DBlob> landscapeVS{ CompileShader(L"Shaders\\Landscape.hlsl", nullptr, "VS", "vs_5_1") };
	ComPtr<ID3DBlob> landscapeHS{ CompileShader(L"Shaders\\Landscape.hlsl", nullptr, "HSControlPoint", "hs_5_1") };
	ComPtr<ID3DBlob> landscapeDS{ CompileShader(L"Shaders\\Landscape.hlsl", nullptr, "DS", "ds_5_1") };
	ComPtr<ID3DBlob> landscapePS{ CompileShader(L"Shaders\\Landscape.hlsl", nullptr, "PS", "ps_5_1") };
	
	ComPtr<ID3DBlob> skyboxVS{ CompileShader(L"Shaders\\Skybox.hlsl", nullptr, "VS", "vs_5_1") };
	ComPtr<ID3DBlob> skyboxPS{ CompileShader(L"Shaders\\Skybox.hlsl", nullptr, "PS", "ps_5_1") };
	
	ComPtr<ID3DBlob> billBoardVS{ CompileShader(L"Shaders\\Billboard.hlsl", nullptr, "VS", "vs_5_1") };
	ComPtr<ID3DBlob> billBoardGS{ CompileShader(L"Shaders\\Billboard.hlsl", nullptr, "GS", "gs_5_1") };
	ComPtr<ID3DBlob> billBoardPS{ CompileShader(L"Shaders\\Billboard.hlsl", nullptr, "PS", "ps_5_1") };

	ComPtr<ID3DBlob> particleVS{ CompileShader(L"Shaders\\Particle.hlsl", nullptr, "VS", "vs_5_1") };
	ComPtr<ID3DBlob> particleGS{ CompileShader(L"Shaders\\Particle.hlsl", nullptr, "GS", "gs_5_1") };
	ComPtr<ID3DBlob> particlePS{ CompileShader(L"Shaders\\Particle.hlsl", nullptr, "PS", "ps_5_1") };

	ComPtr<ID3DBlob> debugVS{ CompileShader(L"Shaders\\DebugTexture.hlsl", nullptr, "VS", "vs_5_1") };
	ComPtr<ID3DBlob> debugPS{ CompileShader(L"Shaders\\DebugTexture.hlsl", nullptr, "PS", "ps_5_1") };

	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaqueDesc{};
	opaqueDesc.InputLayout = { inputLayout.data(), static_cast<UINT>(inputLayout.size()) };
	opaqueDesc.pRootSignature = signature["Default"].Get();
	opaqueDesc.VS = 
	{
		reinterpret_cast<BYTE*>(defaultVS->GetBufferPointer()),
		defaultVS->GetBufferSize()
	};
	opaqueDesc.PS = 
	{
		reinterpret_cast<BYTE*>(defaultPS->GetBufferPointer()),
		defaultPS->GetBufferSize()
	};
	opaqueDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	opaqueDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	opaqueDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	opaqueDesc.SampleMask = UINT_MAX;
	opaqueDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	opaqueDesc.NumRenderTargets = 1;
	opaqueDesc.RTVFormats[0] = app->mBackBufferFormat;
	opaqueDesc.SampleDesc.Count = 1;
	opaqueDesc.SampleDesc.Quality = 0;
	opaqueDesc.DSVFormat = app->mDepthStencilFormat;

	app->GetDevice()->CreateGraphicsPipelineState(&opaqueDesc, IID_PPV_ARGS(&psos["Opaque"]));

	opaqueDesc.VS =
	{
		reinterpret_cast<BYTE*>(debugVS->GetBufferPointer()),
		debugVS->GetBufferSize()
	};
	opaqueDesc.PS =
	{
		reinterpret_cast<BYTE*>(debugPS->GetBufferPointer()),
		debugPS->GetBufferSize()
	};

	app->GetDevice()->CreateGraphicsPipelineState(&opaqueDesc, IID_PPV_ARGS(&psos["Debug"]));

	D3D12_GRAPHICS_PIPELINE_STATE_DESC smDesc{ opaqueDesc };
	smDesc.RasterizerState.DepthBias = 100000;
	smDesc.RasterizerState.DepthBiasClamp = 0.0f;
	smDesc.RasterizerState.SlopeScaledDepthBias = 1.0f;
	smDesc.VS =
	{
		reinterpret_cast<BYTE*>(shadowVS->GetBufferPointer()),
		shadowVS->GetBufferSize()
	};
	smDesc.PS =
	{
		reinterpret_cast<BYTE*>(shadowPS->GetBufferPointer()),
		shadowPS->GetBufferSize()
	};
	smDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
	smDesc.NumRenderTargets = 0;
	
	app->GetDevice()->CreateGraphicsPipelineState(&smDesc, IID_PPV_ARGS(&psos["Shadow"]));

	opaqueDesc.VS =
	{
		reinterpret_cast<BYTE*>(blendVS->GetBufferPointer()),
		blendVS->GetBufferSize()
	};
	opaqueDesc.PS =
	{
		reinterpret_cast<BYTE*>(blendPS->GetBufferPointer()),
		blendPS->GetBufferSize()
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC blendPSODesc{ opaqueDesc };
	
	D3D12_RENDER_TARGET_BLEND_DESC blendRTDesc{};
	blendRTDesc.BlendEnable = true;	// Blend 활성화
	blendRTDesc.LogicOpEnable = false; // 논리 연산자 활성화 ( blend와 같이 쓸 수 없음 )
	blendRTDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA; // 그려'질' 색상 계수
	blendRTDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA; // 그려'져있는' 색상 계수 (INV에 의해 1 - a로 변함 )
	blendRTDesc.BlendOp = D3D12_BLEND_OP_ADD; // RGB 혼합 연산자
	blendRTDesc.SrcBlendAlpha = D3D12_BLEND_ONE; // 그려'질' 색상 알파 계수
	blendRTDesc.DestBlendAlpha = D3D12_BLEND_ZERO; // 그려'져있는' 색상 알파 계수
	blendRTDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD; // 알파 혼합 연산자
	blendRTDesc.LogicOp = D3D12_LOGIC_OP_NOOP; // 논리 연산자 혼합
	blendRTDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;	// 후면 버퍼 색상 채널 결정
	
	blendPSODesc.BlendState.RenderTarget[0] = blendRTDesc;

	blendPSODesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;	// Blend는 CULL모드를 끔

	app->GetDevice()->CreateGraphicsPipelineState(&blendPSODesc, IID_PPV_ARGS(&psos["Blend"]));

	opaqueDesc.VS =
	{
		reinterpret_cast<BYTE*>(landscapeVS->GetBufferPointer()),
		landscapeVS->GetBufferSize()
	};
	opaqueDesc.PS =
	{
		reinterpret_cast<BYTE*>(landscapePS->GetBufferPointer()),
		landscapePS->GetBufferSize()
	};
	opaqueDesc.HS =
	{
		reinterpret_cast<BYTE*>(landscapeHS->GetBufferPointer()),
		landscapeHS->GetBufferSize()
	};
	opaqueDesc.DS =
	{
		reinterpret_cast<BYTE*>(landscapeDS->GetBufferPointer()),
		landscapeDS->GetBufferSize()
	};
	opaqueDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	opaqueDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
	app->GetDevice()->CreateGraphicsPipelineState(&opaqueDesc, IID_PPV_ARGS(&psos["Landscape"]));

	opaqueDesc.HS = {};
	opaqueDesc.DS = {};
	opaqueDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	opaqueDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	opaqueDesc.VS =
	{
		reinterpret_cast<BYTE*>(billBoardVS->GetBufferPointer()),
		billBoardVS->GetBufferSize()
	};
	opaqueDesc.GS =
	{
		reinterpret_cast<BYTE*>(billBoardGS->GetBufferPointer()),
		billBoardGS->GetBufferSize()
	};
	opaqueDesc.PS =
	{
		reinterpret_cast<BYTE*>(billBoardPS->GetBufferPointer()),
		billBoardPS->GetBufferSize()
	};
	opaqueDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
	app->GetDevice()->CreateGraphicsPipelineState(&opaqueDesc, IID_PPV_ARGS(&psos["Billboard"]));

	opaqueDesc.VS =
	{
		reinterpret_cast<BYTE*>(particleVS->GetBufferPointer()),
		particleVS->GetBufferSize()
	};
	opaqueDesc.GS =
	{
		reinterpret_cast<BYTE*>(particleGS->GetBufferPointer()),
		particleGS->GetBufferSize()
	};
	opaqueDesc.PS =
	{
		reinterpret_cast<BYTE*>(particlePS->GetBufferPointer()),
		particlePS->GetBufferSize()
	};
	app->GetDevice()->CreateGraphicsPipelineState(&opaqueDesc, IID_PPV_ARGS(&psos["Particle"]));

	opaqueDesc.GS = {};
	opaqueDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	opaqueDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	opaqueDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	opaqueDesc.VS =
	{
		reinterpret_cast<BYTE*>(skyboxVS->GetBufferPointer()),
		skyboxVS->GetBufferSize()
	};
	opaqueDesc.PS =
	{
		reinterpret_cast<BYTE*>(skyboxPS->GetBufferPointer()),
		skyboxPS->GetBufferSize()
	};

	app->GetDevice()->CreateGraphicsPipelineState(&opaqueDesc, IID_PPV_ARGS(&psos["Skybox"]));
}

void ResourceManager::CreateResources()
{
	D3DApp* app{ D3DApp::GetApp() };
	passCB = std::make_unique<Buffers::UploadBuffer<PassInfomation>>(app->GetDevice(), 1, true);
	shadowCB = std::make_unique<Buffers::UploadBuffer<PassInfomation>>(app->GetDevice(), 1, true);
}

void ResourceManager::CreateShaderResourceView()
{
	ID3D12Device* device{ D3DApp::GetApp()->GetDevice() };
	shadowMap = std::make_unique<ShadowMap>(device, 1024, 768);
	srvIncSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_DESCRIPTOR_HEAP_DESC desc{};
	desc.NumDescriptors = static_cast<UINT>(textures.size()) + 1;
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	desc.NodeMask = 0;
	device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&srvHeap));

	std::vector<Texture*> t(textures.size());
	auto iterator{ textures.begin() };

	for (auto& it : t) {
		it = iterator->second.get();
		++iterator;
	}

	std::sort(t.begin(), t.end(), [](const Texture* lhs, const Texture* rhs) {
		return lhs->GetSrvIndex() < rhs->GetSrvIndex();
		});
	
	//D3D12_
	D3D12_SHADER_RESOURCE_VIEW_DESC srv2DDesc{};
	srv2DDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srv2DDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srv2DDesc.Texture2D.MostDetailedMip = 0;
	srv2DDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	D3D12_SHADER_RESOURCE_VIEW_DESC srv3DDesc{ srv2DDesc };
	srv3DDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
	srv3DDesc.TextureCube.MostDetailedMip = 0;
	srv3DDesc.TextureCube.ResourceMinLODClamp = 0.0f;

	D3D12_SHADER_RESOURCE_VIEW_DESC srv2DArrayDesc{ srv2DDesc };
	srv2DArrayDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
	srv2DArrayDesc.Texture2DArray.MostDetailedMip = 0;
	srv2DArrayDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;

	D3D12_CPU_DESCRIPTOR_HANDLE handle{ srvHeap->GetCPUDescriptorHandleForHeapStart() };
	D3D12_GPU_DESCRIPTOR_HANDLE gHandle{ srvHeap->GetGPUDescriptorHandleForHeapStart() };

	for (auto& it : t) {
		UINT16 size = it->GetResource()->GetDesc().DepthOrArraySize;
		
		if (size == 1) {
			srv2DDesc.Texture2D.MipLevels = it->GetResource()->GetDesc().MipLevels;
			srv2DDesc.Format = it->GetResource()->GetDesc().Format;
			device->CreateShaderResourceView(it->GetResource(), &srv2DDesc, handle);
		}
		else if (size == 6) {
			srv3DDesc.TextureCube.MipLevels = it->GetResource()->GetDesc().MipLevels;
			srv3DDesc.Format = it->GetResource()->GetDesc().Format;
			device->CreateShaderResourceView(it->GetResource(), &srv3DDesc, handle);
		}
		else {
			srv2DArrayDesc.Texture2DArray.MipLevels = it->GetResource()->GetDesc().MipLevels;
			srv2DArrayDesc.Texture2DArray.ArraySize = size;
			srv2DArrayDesc.Format = it->GetResource()->GetDesc().Format;
			device->CreateShaderResourceView(it->GetResource(), &srv2DArrayDesc, handle);
		}
		it->SetHandle(gHandle);
		handle.ptr += srvIncSize;
		gHandle.ptr += srvIncSize;
	}
	srv2DDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srv2DDesc.Texture2D.MipLevels = 1;
	device->CreateShaderResourceView(shadowMap->GetResource(), &srv2DDesc, handle);
	shadowMap->SetSrvHandle(gHandle);
}
