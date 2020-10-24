#include "stdafx.h"
#include "TransformComponent.h"

void TransformComponent::SetPosition(const DirectX::XMFLOAT3& pos)
{
	position = pos;
}

void TransformComponent::SetRotation(const DirectX::XMFLOAT3& rot)
{
	rotation = rot;
}

void TransformComponent::SetTransform(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& rot)
{
	SetPosition(pos);
	SetRotation(rot);
}
