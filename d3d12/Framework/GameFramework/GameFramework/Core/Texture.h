#pragma once

class Texture
{
public:
	Texture(const std::wstring_view& fileName);
	~Texture();

	void ReleaseUploadBuffer();
	void SetSrvIndex(size_t idx);

	size_t GetSrvIndex() const;
	ID3D12Resource* GetResource();

	void SetHandle(D3D12_GPU_DESCRIPTOR_HANDLE h);

	void BindingResource(ID3D12GraphicsCommandList* cmdList);
private:

	Microsoft::WRL::ComPtr<ID3D12Resource> tBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> tUploadBuffer;

	size_t srvIndex{};
	D3D12_GPU_DESCRIPTOR_HANDLE handle{};
};

