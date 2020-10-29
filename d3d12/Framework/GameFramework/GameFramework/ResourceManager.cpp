#include "stdafx.h"
#include "ResourceManager.h"
#include "d3dx12.h"

using namespace Microsoft::WRL;

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

}

void ResourceManager::Init()
{
	CreateRootSignature();
	CreatePSO();
}

void ResourceManager::BindingResource(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->SetPipelineState(psos["Opaque"].Get());
	cmdList->SetGraphicsRootSignature(rootSignature.Get());
}

void ResourceManager::CreateRootSignature()
{
	ID3D12Device* device{ D3DApp::GetApp()->GetDevice() };
	D3D12_ROOT_PARAMETER param[4]{};

	param[0].Descriptor.ShaderRegister = 0;
	param[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	// WORLD

	param[1].Descriptor.ShaderRegister = 1;
	param[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	// Materials

	param[2].Descriptor.ShaderRegister = 2;
	param[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;	// Pass

	D3D12_DESCRIPTOR_RANGE srvRange{};
	srvRange.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	srvRange.NumDescriptors = 1;

	param[3].DescriptorTable.NumDescriptorRanges = 1;
	param[3].DescriptorTable.pDescriptorRanges = &srvRange;

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
	desc.NumParameters = 1;
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

	ComPtr<ID3DBlob> DefaultVS{ CompileShader(L"Shaders\\Default.hlsl", nullptr, "VS", "vs_5_1") };
	ComPtr<ID3DBlob> DefaultPS{ CompileShader(L"Shaders\\Default.hlsl", nullptr, "PS", "ps_5_1") };

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
		reinterpret_cast<BYTE*>(DefaultVS->GetBufferPointer()),
		DefaultVS->GetBufferSize()
	};
	opaqueDesc.PS = 
	{
		reinterpret_cast<BYTE*>(DefaultPS->GetBufferPointer()),
		DefaultPS->GetBufferSize()
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
}
