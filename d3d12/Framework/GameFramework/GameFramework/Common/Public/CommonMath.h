#pragma once

class CommonMath {
public:
	static DirectX::XMFLOAT3 CrossProduct(DirectX::XMFLOAT3& v1, DirectX::XMFLOAT3& v2, bool normalize = true);
	static DirectX::XMFLOAT4X4 Multiply(const DirectX::XMFLOAT4X4& mat1, const DirectX::XMFLOAT4X4& mat2);
	static DirectX::XMFLOAT4X4 Multiply(const DirectX::XMFLOAT4X4& mat1, const DirectX::XMMATRIX& mat2);
	static DirectX::XMFLOAT4X4 Multiply(const DirectX::XMMATRIX& mat1, const DirectX::XMFLOAT4X4& mat2);
	static DirectX::XMFLOAT3 TransformCoord(const DirectX::XMFLOAT3& vector, const DirectX::XMMATRIX& mat);
	static DirectX::XMFLOAT3 TransformCoord(const DirectX::XMFLOAT3& vector, const DirectX::XMFLOAT4X4& mat);
	static DirectX::XMFLOAT3 TransformNormal(const DirectX::XMFLOAT3& vector, const DirectX::XMMATRIX& mat);
	static DirectX::XMFLOAT3 Vector3Normalize(const DirectX::XMFLOAT3& vector);
	static DirectX::XMFLOAT3 MultiplyScalar(const DirectX::XMFLOAT3& vector, float scalar);
	static DirectX::XMFLOAT3 Add(const DirectX::XMFLOAT3& vector1, const DirectX::XMFLOAT3& vector2);
	static DirectX::XMFLOAT3 Add(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2, float scalar);
	static DirectX::XMFLOAT3 Subtract(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2);
	static float Length(const DirectX::XMFLOAT3& v);
	static DirectX::XMFLOAT4X4 LookAtLH(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& lookAt, const DirectX::XMFLOAT3& up);
	static float Dot(const DirectX::XMFLOAT3& v1, const DirectX::XMFLOAT3& v2);
};

