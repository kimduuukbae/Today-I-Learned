#include "stdafx.h"
#include "ShadowMap.h"

ShadowMap::ShadowMap(ID3D12Device* device, uint32_t width, uint32_t height)
	: width{ width }, height{ height }, format { DXGI_FORMAT_R24G8_TYPELESS }
{
	this->device = device;
	viewport = { 0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f, 1.0f };
	scissorRect = { 0, 0, static_cast<int>(width), static_cast<int>(height) };

	D3D12_RESOURCE_DESC texDesc{};
	texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texDesc.Alignment = 0;
	texDesc.Width = this->width;
	texDesc.Height = this->height;
	texDesc.DepthOrArraySize = 1;
	texDesc.MipLevels = 1;
	texDesc.Format = format;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear{};
	optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;

	D3D12_HEAP_PROPERTIES prop{};
	prop.Type = D3D12_HEAP_TYPE_DEFAULT;

	device->CreateCommittedResource(
		&prop,
		D3D12_HEAP_FLAG_NONE,
		&texDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		&optClear,
		IID_PPV_ARGS(&shadowMap));

	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.NodeMask = 0;
	device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(dsvHeap.GetAddressOf()));
	
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.Texture2D.MipSlice = 0;
	device->CreateDepthStencilView(shadowMap.Get(), &dsvDesc, dsvHeap->GetCPUDescriptorHandleForHeapStart());
}

ShadowMap::~ShadowMap()
{
	device = nullptr;
	shadowMap = nullptr;
	dsvHeap = nullptr;
}

uint32_t ShadowMap::GetWidth() const
{
	return width;
}

uint32_t ShadowMap::GetHeight() const
{
	return height;
}

ID3D12Resource* ShadowMap::GetResource()
{
	return shadowMap.Get();
}

D3D12_GPU_DESCRIPTOR_HANDLE ShadowMap::srvHandle() const
{
	return handle;
}

D3D12_CPU_DESCRIPTOR_HANDLE ShadowMap::depthHandle() const
{
	return dsvHeap->GetCPUDescriptorHandleForHeapStart();
}

const D3D12_VIEWPORT& ShadowMap::GetViewport() const
{
	return viewport;
}

const D3D12_RECT& ShadowMap::GetRect() const
{
	return scissorRect;
}

void ShadowMap::Resize(uint32_t width, uint32_t height)
{
}

void ShadowMap::SetSrvHandle(D3D12_GPU_DESCRIPTOR_HANDLE h)
{
	handle = h;
}
