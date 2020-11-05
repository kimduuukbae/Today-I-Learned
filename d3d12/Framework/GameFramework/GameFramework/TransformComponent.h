#pragma once

#include "Common.h"

class TransformComponent
{
public:
	TransformComponent();
	~TransformComponent();

	void SetPosition(const DirectX::XMFLOAT3& pos);
	void SetRotation(const DirectX::XMFLOAT3& rot);
	void SetTransform(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& rot);
	DirectX::XMFLOAT3 GetPosition();


	D3D12_GPU_VIRTUAL_ADDRESS GetResourceAddress();

private:
	DirectX::XMFLOAT3 position{};
	DirectX::XMFLOAT3 rotation{};

	DirectX::XMFLOAT4X4 worldMatrix{ Math::Identity4x4() };
	std::unique_ptr<Buffers::UploadBuffer<DirectX::XMFLOAT4X4>> cBuffer;

	void UpdateMatrix();
};

