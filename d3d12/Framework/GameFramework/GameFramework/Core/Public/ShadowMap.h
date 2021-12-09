#pragma once

class ShadowMap
{
public:
	ShadowMap(ID3D12Device* device, uint32_t width, uint32_t height);
	~ShadowMap();

	uint32_t GetWidth() const;
	uint32_t GetHeight() const;
	ID3D12Resource* GetResource();
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandle() const;
	D3D12_CPU_DESCRIPTOR_HANDLE depthHandle() const;

	const D3D12_VIEWPORT& GetViewport() const;
	const D3D12_RECT& GetRect() const;

	void Resize(uint32_t width, uint32_t height);
	void SetSrvHandle(D3D12_GPU_DESCRIPTOR_HANDLE h);
private:
	D3D12_VIEWPORT viewport;
	D3D12_RECT scissorRect;

	uint32_t width;
	uint32_t height;
	DXGI_FORMAT format;

	ID3D12Device* device;

	Microsoft::WRL::ComPtr<ID3D12Resource> shadowMap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap;

	D3D12_GPU_DESCRIPTOR_HANDLE handle;
};

