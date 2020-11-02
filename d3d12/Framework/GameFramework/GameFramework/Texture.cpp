#include "stdafx.h"
#include "Texture.h"
#include "DDSTextureLoader.h"

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