#include "stdafx.h"
#include "TransformComponent.h"

using namespace DirectX;

TransformComponent::TransformComponent()
{
	cBuffer = std::make_unique<Buffers::UploadBuffer<MatrixInformation>>(D3DApp::GetApp()->GetDevice(), 1, true);
	matrix.textureTransform = { 1.0f, 1.0f };
	matrix.worldMatrix = Math::Identity4x4();
	
	cBuffer->CopyData(matrix);
}

TransformComponent::~TransformComponent()
{
}

void TransformComponent::SetPosition(const DirectX::XMFLOAT3& pos)
{
	position = pos;

	matrix.worldMatrix._41 = pos.x;
	matrix.worldMatrix._42 = pos.y;
	matrix.worldMatrix._43 = pos.z;

	
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

	matrix.worldMatrix._41 = pos.x;
	matrix.worldMatrix._42 = pos.y;
	matrix.worldMatrix._43 = pos.z;

	rotation = rot;
	UpdateMatrix();
}

void TransformComponent::SetTextureTransformSize(float xSize, float ySize)
{
	matrix.textureTransform.x = xSize;
	matrix.textureTransform.y = ySize;

	UpdateMatrix();
}

DirectX::XMFLOAT3 TransformComponent::GetPosition()
{
	return position;
}

DirectX::XMFLOAT4X4& TransformComponent::GetTransformDirect()
{
	return matrix.worldMatrix;
}

D3D12_GPU_VIRTUAL_ADDRESS TransformComponent::GetResourceAddress()
{
	return cBuffer->GetResource()->GetGPUVirtualAddress();
}

void TransformComponent::RotateX(float angle)
{
	XMMATRIX rot{ DirectX::XMMatrixRotationAxis(XMLoadFloat3(&right), DirectX::XMConvertToRadians(angle)) };
	look = Math::TransformNormal(look, rot);
	up = Math::TransformNormal(up, rot);

	BasisNormalize();
}

void TransformComponent::RotateY(float angle)
{
	XMMATRIX rot{ DirectX::XMMatrixRotationAxis(XMLoadFloat3(&up), DirectX::XMConvertToRadians(angle)) };
	look = Math::TransformNormal(look, rot);
	right = Math::TransformNormal(right, rot);

	BasisNormalize();
}

void TransformComponent::RotateZ(float angle)
{
	XMMATRIX rot{ DirectX::XMMatrixRotationAxis(XMLoadFloat3(&look), DirectX::XMConvertToRadians(angle)) };
	right = Math::TransformNormal(right, rot);
	up = Math::TransformNormal(up, rot);

	BasisNormalize();
}

void TransformComponent::Forward(float distance)
{
	position = Math::Add(position, Math::MultiplyScalar(look, distance));
	matrix.worldMatrix._41 = position.x;
	matrix.worldMatrix._42 = position.y;
	matrix.worldMatrix._43 = position.z;
	UpdateMatrix();
}

void TransformComponent::Right(float distance)
{
	position = Math::Add(position, Math::MultiplyScalar(right, distance));
	matrix.worldMatrix._41 = position.x;
	matrix.worldMatrix._42 = position.y;
	matrix.worldMatrix._43 = position.z;
	UpdateMatrix();
}

void TransformComponent::UpdateMatrix()
{
	MatrixInformation mat{ .textureTransform = matrix.textureTransform };

	DirectX::XMStoreFloat4x4(&mat.worldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&matrix.worldMatrix)));
	cBuffer->CopyData(mat);
}

DirectX::XMFLOAT3 TransformComponent::GetRight()
{
	return right;
}

DirectX::XMFLOAT3 TransformComponent::GetUp()
{
	return up;
}

DirectX::XMFLOAT3 TransformComponent::GetLook()
{
	return look;
}

void TransformComponent::BasisNormalize()
{
	look = Math::Vector3Normalize(look);
	up = Math::CrossProduct(look, right, true);
	right = Math::CrossProduct(up, look, true);

	XMFLOAT4X4& worldMatrix{ matrix.worldMatrix };

	worldMatrix._11 = right.x; worldMatrix._12 = right.y; worldMatrix._13 = right.z;
	worldMatrix._21 = up.x; worldMatrix._22 = up.y; worldMatrix._23 = up.z;
	worldMatrix._31 = look.x; worldMatrix._32 = look.y; worldMatrix._33 = look.z;
	
	UpdateMatrix();
}
