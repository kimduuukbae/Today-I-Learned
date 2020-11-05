#include "stdafx.h"
#include "TransformComponent.h"

using namespace DirectX;

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
	worldMatrix._41 = pos.x;
	worldMatrix._42 = pos.y;
	worldMatrix._43 = pos.z;

	position = pos;
	UpdateMatrix();
}

void TransformComponent::SetRotation(const DirectX::XMFLOAT3& rot)
{
	rotation = rot;
	UpdateMatrix();
}

void TransformComponent::SetTransform(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& rot)
{
	position = pos;
	rotation = rot;
	UpdateMatrix();
}

DirectX::XMFLOAT3 TransformComponent::GetPosition()
{
	return position;
}

D3D12_GPU_VIRTUAL_ADDRESS TransformComponent::GetResourceAddress()
{
	return cBuffer->GetResource()->GetGPUVirtualAddress();
}

void TransformComponent::UpdateMatrix()
{
	worldMatrix._41 = position.x;
	worldMatrix._42 = position.y;
	worldMatrix._43 = position.z;

	DirectX::XMFLOAT4X4 m{worldMatrix};

	XMStoreFloat4x4(&m, XMMatrixTranspose(XMLoadFloat4x4(&m)));
	cBuffer->CopyData(0, m);
}
