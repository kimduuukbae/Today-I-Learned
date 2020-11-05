#include "stdafx.h"
#include "ResourceManager.h"
#include "d3dx12.h"
#include "CameraComponent.h"
#include "Texture.h"
#include <filesystem>

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

void ResourceManager::BindingResource(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->SetGraphicsRootSignature(rootSignature.Get());

	ID3D12DescriptorHeap* descriptorHeaps[] = { srvHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	//같은 타입의 힙은 하나만 달 수 있음!

	PassInfomation pass;

	XMMATRIX view{ mainCam->GetView() };
	XMMATRIX proj{ mainCam->GetProj() };
	XMVECTOR pos{ mainCam->GetPosition() };
	XMStoreFloat4x4(&pass.viewMatrix, XMMatrixTranspose(view));
	XMStoreFloat4x4(&pass.projMatrix, XMMatrixTranspose(proj));
	pass.eyePosition = mainCam->GetPosition3f();

	XMStoreFloat4x4(&pass.viewProj, XMMatrixTranspose(XMMatrixMultiply(view, proj)));
	passCB->CopyData(0, pass);
	cmdList->SetGraphicsRootConstantBufferView(1, passCB->GetResource()->GetGPUVirtualAddress());
}

void ResourceManager::ReleaseUploadBuffer()
{
	for (auto& it : textures)
		it.second->ReleaseUploadBuffer();
}

void ResourceManager::CreateRootSignature()
{
	ID3D12Device* device{ D3DApp::GetApp()->GetDevice() };
	D3D12_ROOT_PARAMETER param[3]{};

	param[0].Descriptor.ShaderRegister = 0;
	param[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	// WORLD
	param[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	param[1].Descriptor.ShaderRegister = 1;
	param[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	// Pass
	param[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	/*param[2].Descriptor.ShaderRegister = 2;
	param[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	// Pass
	*/
	D3D12_DESCRIPTOR_RANGE srvRange{};
	srvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvRange.NumDescriptors = 2;

	param[2].DescriptorTable.NumDescriptorRanges = 1;
	param[2].DescriptorTable.pDescriptorRanges = &srvRange;
	param[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_STATIC_SAMPLER_DESC ssDesc{};

	std::array<CD3DX12_STATIC_SAMPLER_DESC, 6> ssamples{
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
			8)                                // maxAnisotropy
	};

	D3D12_ROOT_SIGNATURE_DESC desc{};
	desc.pParameters = param;
	desc.NumParameters = 3;
	desc.NumStaticSamplers = 6;
	desc.pStaticSamplers = ssamples.data();
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ComPtr<ID3DBlob> serializeBlob{}, errorBlob{};
	D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, serializeBlob.GetAddressOf(), errorBlob.GetAddressOf());
	device->CreateRootSignature(0, serializeBlob->GetBufferPointer(), serializeBlob->GetBufferSize()
		, IID_PPV_ARGS(rootSignature.GetAddressOf()));
}

void ResourceManager::CreatePSO()
{
	D3DApp* app{ D3DApp::GetApp() };

	ComPtr<ID3DBlob> defaultVS{ CompileShader(L"Shaders\\Default.hlsl", nullptr, "VS", "vs_5_1") };
	ComPtr<ID3DBlob> defaultPS{ CompileShader(L"Shaders\\Default.hlsl", nullptr, "PS", "ps_5_1") };
	ComPtr<ID3DBlob> landscapeVS{ CompileShader(L"Shaders\\Landscape.hlsl", nullptr, "VS", "vs_5_1") };
	ComPtr<ID3DBlob> landscapePS{ CompileShader(L"Shaders\\Landscape.hlsl", nullptr, "PS", "ps_5_1") };
	ComPtr<ID3DBlob> skyboxVS{ CompileShader(L"Shaders\\Skybox.hlsl", nullptr, "VS", "vs_5_1") };
	ComPtr<ID3DBlob> skyboxPS{ CompileShader(L"Shaders\\Skybox.hlsl", nullptr, "PS", "ps_5_1") };

	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC opaqueDesc{};
	opaqueDesc.InputLayout = { inputLayout.data(), static_cast<UINT>(inputLayout.size()) };
	opaqueDesc.pRootSignature = rootSignature.Get();
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
		reinterpret_cast<BYTE*>(landscapeVS->GetBufferPointer()),
		landscapeVS->GetBufferSize()
	};
	opaqueDesc.PS =
	{
		reinterpret_cast<BYTE*>(landscapePS->GetBufferPointer()),
		landscapePS->GetBufferSize()
	};

	app->GetDevice()->CreateGraphicsPipelineState(&opaqueDesc, IID_PPV_ARGS(&psos["Landscape"]));

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
}

void ResourceManager::CreateShaderResourceView()
{
	ID3D12Device* device{ D3DApp::GetApp()->GetDevice() };
	srvIncSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	D3D12_DESCRIPTOR_HEAP_DESC desc{};
	desc.NumDescriptors = static_cast<UINT>(textures.size());
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
		it->SetHandle(gHandle);
		handle.ptr += srvIncSize;
		gHandle.ptr += srvIncSize;
	}
}
