#include "stdafx.h"
#include "TransformComponent.h"

TransformComponent::TransformComponent()
{
	cBuffer = std::make_unique<Buffers::UploadBuffer<DirectX::XMFLOAT4X4>>(D3DApp::GetApp()->GetDevice(), 1, true);
	cBuffer->CopyData(0, worldMatrix);
}

TransformComponent::~TransformComponent()
{
}

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

D3D12_GPU_VIRTUAL_ADDRESS TransformComponent::GetResourceAddress()
{
	return cBuffer->GetResource()->GetGPUVirtualAddress();
}
