#pragma once

class Texture
{
public:
	Texture(const std::wstring_view& fileName);
	~Texture();

	void ReleaseUploadBuffer();

private:

	Microsoft::WRL::ComPtr<ID3D12Resource> tBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> tUploadBuffer;
};

