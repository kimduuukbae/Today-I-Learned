#pragma once

#include "Common.h"

class CameraComponent
{
public:
	CameraComponent();
	~CameraComponent();

	DirectX::XMVECTOR GetPosition() const;
	DirectX::XMFLOAT3 GetPosition3f() const;
	void SetPosition(float x, float y, float z);
	void SetPosition(const DirectX::XMFLOAT3& v);

	// Get camera basis vectors.
	DirectX::XMVECTOR GetRight() const;
	DirectX::XMFLOAT3 GetRight3f() const;
	DirectX::XMVECTOR GetUp() const;
	DirectX::XMFLOAT3 GetUp3f() const;
	DirectX::XMVECTOR GetLook() const;
	DirectX::XMFLOAT3 GetLook3f() const;

	float GetNearZ() const;
	float GetFarZ() const;
	float GetAspect() const;
	float GetFovY() const;
	float GetFovX() const;

	void LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp);
	void LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up);

	DirectX::XMMATRIX GetView()const;
	DirectX::XMMATRIX GetProj()const;

	DirectX::XMFLOAT4X4 GetView4x4f()const;
	DirectX::XMFLOAT4X4 GetProj4x4f()const;

	void UpdateMatrix();
	void SetLens(float fovY, float aspect, float nZ, float fZ);

	// Get near and far plane dimensions in view space coordinates.
	float GetNearWindowWidth()const;
	float GetNearWindowHeight()const;
	float GetFarWindowWidth()const;
	float GetFarWindowHeight()const;

	void Walk(float d);
	void Strafe(float d);
	void Pitch(float angle);
	void RotateY(float angle);
private:
	DirectX::XMFLOAT3 position { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 right { 1.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 up { 0.0f, 1.0f, 0.0f };
	DirectX::XMFLOAT3 look { 0.0f, 0.0f, 1.0f };

	float nearZ{ 0.0f };
	float farZ{ 0.0f };
	float aspect{ 0.0f };
	float fovY{ 0.0f };

	DirectX::XMFLOAT4X4 viewMatrix{ Math::Identity4x4() };
	DirectX::XMFLOAT4X4 projMatrix{ Math::Identity4x4() };

	float mNearWindowHeight{ 0.0f };
	float mFarWindowHeight{ 0.0f };
};

