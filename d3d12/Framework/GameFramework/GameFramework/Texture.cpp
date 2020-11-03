#include "stdafx.h"
#include "Texture.h"
#include "DDSTextureLoader.h"
#include "Common.h"

Texture::Texture(const std::wstring_view& fileName)
{
	D3DApp* app{ D3DApp::GetApp() };
	DirectX::CreateDDSTextureFromFile12(app->GetDevice(), app->GetCommandList(), fileName.data(), tBuffer, tUploadBuffer);
}

Texture::~Texture()
{
	tBuffer = nullptr;
	tUploadBuffer = nullptr;
}

void Texture::ReleaseUploadBuffer()
{
	tUploadBuffer = nullptr;
}

void Texture::SetSrvIndex(size_t idx)
{
	srvIndex = idx;
}

size_t Texture::GetSrvIndex() const
{
	return srvIndex;
}

ID3D12Resource* Texture::GetResource()
{
	return tBuffer.Get();
}

void Texture::SetHandle(D3D12_GPU_DESCRIPTOR_HANDLE h)
{
	handle = h;
}

void Texture::BindingResource(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->SetGraphicsRootDescriptorTable(2, handle);
}