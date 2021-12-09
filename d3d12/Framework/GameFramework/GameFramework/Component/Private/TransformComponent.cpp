#include "stdafx.h"
#include "TransformComponent.h"

using namespace DirectX;

TransformComponent::TransformComponent()
{
	cBuffer = std::make_unique<Buffers::UploadBuffer<MatrixInformation>>(D3DApp::GetApp()->GetDevice(), 1, true);
	matrix.textureTransform = { 1.0f, 1.0f };
	matrix.worldMatrix = CommonMath::Identity4x4();
	
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
	look = CommonMath::TransformNormal(look, rot);
	up = CommonMath::TransformNormal(up, rot);

	BasisNormalize();
}

void TransformComponent::RotateY(float angle)
{
	XMMATRIX rot{ DirectX::XMMatrixRotationAxis(XMLoadFloat3(&up), DirectX::XMConvertToRadians(angle)) };
	look = CommonMath::TransformNormal(look, rot);
	right = CommonMath::TransformNormal(right, rot);

	BasisNormalize();
}

void TransformComponent::RotateZ(float angle)
{
	XMMATRIX rot{ DirectX::XMMatrixRotationAxis(XMLoadFloat3(&look), DirectX::XMConvertToRadians(angle)) };
	right = CommonMath::TransformNormal(right, rot);
	up = CommonMath::TransformNormal(up, rot);

	BasisNormalize();
}

void TransformComponent::Forward(float distance)
{
	position = CommonMath::Add(position, CommonMath::MultiplyScalar(look, distance));
	matrix.worldMatrix._41 = position.x;
	matrix.worldMatrix._42 = position.y;
	matrix.worldMatrix._43 = position.z;
	UpdateMatrix();
}

void TransformComponent::Right(float distance)
{
	position = CommonMath::Add(position, CommonMath::MultiplyScalar(right, distance));
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

void TransformComponent::SetRight(const DirectX::XMFLOAT3& r)
{
	right = r;
}

void TransformComponent::SetUp(const DirectX::XMFLOAT3& u)
{
	up = u;
}

void TransformComponent::SetLook(const DirectX::XMFLOAT3& l)
{
	look = l;
}

void TransformComponent::SetBasisVector(const DirectX::XMFLOAT3& r, const DirectX::XMFLOAT3& u, const DirectX::XMFLOAT3& l)
{
	SetRight(r);
	SetUp(u);
	SetLook(l);
}

void TransformComponent::BasisNormalize()
{
	look = CommonMath::Vector3Normalize(look);
	right = CommonMath::CrossProduct(up, look, true);
	up = CommonMath::CrossProduct(look, right, true);

	XMFLOAT4X4& worldMatrix{ matrix.worldMatrix };

	worldMatrix._11 = right.x; worldMatrix._12 = right.y; worldMatrix._13 = right.z;
	worldMatrix._21 = up.x; worldMatrix._22 = up.y; worldMatrix._23 = up.z;
	worldMatrix._31 = look.x; worldMatrix._32 = look.y; worldMatrix._33 = look.z;
	
	UpdateMatrix();
}
