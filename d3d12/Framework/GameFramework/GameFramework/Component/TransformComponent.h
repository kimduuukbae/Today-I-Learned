#pragma once

#include "Common.h"
#include "IComponent.h"

class TransformComponent : public IComponent
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

	void SetRight(const DirectX::XMFLOAT3& r);
	void SetUp(const DirectX::XMFLOAT3& u);
	void SetLook(const DirectX::XMFLOAT3& l);
	void SetBasisVector(const DirectX::XMFLOAT3& r, const DirectX::XMFLOAT3& u, const DirectX::XMFLOAT3& l);

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

