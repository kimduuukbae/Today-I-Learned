#pragma once

#include "Common.h"

class TransformComponent
{
public:

	void SetPosition(const DirectX::XMFLOAT3& pos);
	void SetRotation(const DirectX::XMFLOAT3& rot);
	void SetTransform(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& rot);

private:
	DirectX::XMFLOAT3 position{};
	DirectX::XMFLOAT3 rotation{};

	DirectX::XMFLOAT4X4 worldMatrix{ Math::Identity4x4() };
};

