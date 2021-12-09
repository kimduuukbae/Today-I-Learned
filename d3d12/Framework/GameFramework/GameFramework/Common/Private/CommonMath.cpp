#include "stdafx.h"
#include "CommonMath.h"

DirectX::XMFLOAT3 CommonMath::CrossProduct(DirectX::XMFLOAT3& v1, DirectX::XMFLOAT3& v2, bool normalize)
{
	DirectX::XMFLOAT3 result;
	if (normalize)
		DirectX::XMStoreFloat3(&result,
			DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&v1), DirectX::XMLoadFloat3(&v2))));
	else
		DirectX::XMStoreFloat3(&result, DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&v1), DirectX::XMLoadFloat3(&v2)));
	return result;
}

DirectX::XMFLOAT4X4 CommonMath::Multiply(const DirectX::XMFLOAT4X4& mat1,
	const DirectX::XMFLOAT4X4& mat2)
{
	DirectX::XMFLOAT4X4 result;
	DirectX::XMStoreFloat4x4(&result, DirectX::XMLoadFloat4x4(&mat1) * DirectX::XMLoadFloat4x4(&mat2));
	return result;
}

DirectX::XMFLOAT4X4 CommonMath::Multiply(const DirectX::XMFLOAT4X4& mat1,
	const DirectX::XMMATRIX& mat2)
{
	DirectX::XMFLOAT4X4 result;
	DirectX::XMStoreFloat4x4(&result, DirectX::XMLoadFloat4x4(&mat1) * mat2);
	return result;
}

DirectX::XMFLOAT4X4 CommonMath::Multiply(const DirectX::XMMATRIX& mat1,
	const DirectX::XMFLOAT4X4& mat2)
{
	DirectX::XMFLOAT4X4 result;
	DirectX::XMStoreFloat4x4(&result, mat1 * DirectX::XMLoadFloat4x4(&mat2));
	return result;
}

DirectX::XMFLOAT3 CommonMath::TransformCoord(const DirectX::XMFLOAT3& vector,
	const DirectX::XMMATRIX& mat)
{
	DirectX::XMFLOAT3 xmf3Result;
	DirectX::XMStoreFloat3(&xmf3Result, DirectX::XMVector3TransformCoord(DirectX::XMLoadFloat3(&vector), mat));
	return(xmf3Result);
}

DirectX::XMFLOAT3 CommonMath::TransformCoord(const DirectX::XMFLOAT3& vector,
	const DirectX::XMFLOAT4X4& mat)
{
	return(TransformCoord(vector, DirectX::XMLoadFloat4x4(&mat)));
}

DirectX::XMFLOAT3 CommonMath::TransformNormal(const DirectX::XMFLOAT3& vector,
	const DirectX::XMMATRIX& mat)
{
	DirectX::XMFLOAT3 result;
	DirectX::XMStoreFloat3(&result, XMVector3TransformNormal(DirectX::XMLoadFloat3(&vector), mat));
	return result;
}

DirectX::XMFLOAT3 CommonMath::Vector3Normalize(const DirectX::XMFLOAT3& vector)
{
	DirectX::XMFLOAT3 result;
	DirectX::XMStoreFloat3(&result, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&vector)));
	return result;
}

DirectX::XMFLOAT3 CommonMath::MultiplyScalar(const DirectX::XMFLOAT3& vector, float scalar)
{
	DirectX::XMFLOAT3 result;
	DirectX::XMStoreFloat3(&result, DirectX::XMVectorScale(DirectX::XMLoadFloat3(&vector), scalar));
	return result;
}

DirectX::XMFLOAT3 CommonMath::Add(const DirectX::XMFLOAT3& vector1, const DirectX::XMFLOAT3& vector2)
{
	DirectX::XMFLOAT3 result;
	DirectX::XMStoreFloat3(&result, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&vector1), DirectX::XMLoadFloat3(&vector2)));
	return result;
}

DirectX::XMFLOAT3 CommonMath::Add(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2, float scalar)
{
	DirectX::XMFLOAT3 result;
	DirectX::XMStoreFloat3(&result, DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&v1),
		DirectX::XMVectorMultiply(DirectX::XMLoadFloat3(&v2), DirectX::XMVectorReplicate(scalar))));
	return result;
}

DirectX::XMFLOAT3 CommonMath::Subtract(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2)
{
	DirectX::XMFLOAT3 result;
	DirectX::XMStoreFloat3(&result, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&v1), DirectX::XMLoadFloat3(&v2)));
	return result;
}

float CommonMath::Length(const DirectX::XMFLOAT3& v)
{
	DirectX::XMFLOAT3 result;
	DirectX::XMStoreFloat3(&result, DirectX::XMVector3Length(DirectX::XMLoadFloat3(&v)));
	return result.x;
}

DirectX::XMFLOAT4X4 CommonMath::LookAtLH(const DirectX::XMFLOAT3& eye,
	const DirectX::XMFLOAT3& lookAt,
	const DirectX::XMFLOAT3& up)
{
	DirectX::XMFLOAT4X4 result;
	DirectX::XMStoreFloat4x4(&result, DirectX::XMMatrixLookAtLH(DirectX::XMLoadFloat3(&eye),
		DirectX::XMLoadFloat3(&lookAt), DirectX::XMLoadFloat3(&up)));
	return result;
}

float CommonMath::Dot(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2)
{
	DirectX::XMFLOAT3 result;
	DirectX::XMStoreFloat3(&result, DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&v1), DirectX::XMLoadFloat3(&v2)));
	return result.x;
}