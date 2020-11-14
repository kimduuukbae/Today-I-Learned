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
	void SetTextureTransformSize(float xSize, float ySize);
	DirectX::XMFLOAT3 GetPosition();

	DirectX::XMFLOAT4X4& GetTransformDirect();

	D3D12_GPU_VIRTUAL_ADDRESS GetResourceAddress();

	void RotateX(float angle);
	void RotateY(float angle);
	void RotateZ(float angle);

	void Forward(float distance);
	void Right(float distance);

	void UpdateMatrix();


	DirectX::XMFLOAT3 GetRight();
	DirectX::XMFLOAT3 GetUp();
	DirectX::XMFLOAT3 GetLook();
private:
	DirectX::XMFLOAT3 position{};
	DirectX::XMFLOAT3 rotation{};

	DirectX::XMFLOAT3 right{ 1.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 up{ 0.0f, 1.0f, 0.0f };
	DirectX::XMFLOAT3 look{ 0.0f, 0.0f, 1.0f };

	struct MatrixInformation
	{
		DirectX::XMFLOAT4X4 worldMatrix;
		DirectX::XMFLOAT2 textureTransform;
	};
	MatrixInformation matrix;

	std::unique_ptr<Buffers::UploadBuffer<MatrixInformation>> cBuffer;


	void BasisNormalize();
};

