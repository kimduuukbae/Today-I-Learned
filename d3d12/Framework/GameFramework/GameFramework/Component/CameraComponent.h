#pragma once

#include "Common.h"

class CameraComponent
{
public:
	CameraComponent();
	virtual ~CameraComponent();

	Vector3 GetPosition3f() const;
	Vector3 GetPosition3fOffset() const;
	void SetPosition(float x, float y, float z);
	void SetPosition(const Vector3& v);

	// Get camera basis vectors.
	Vector3 GetRight3f() const;
	Vector3 GetUp3f() const;
	Vector3 GetLook3f() const;

	float GetNearZ() const;
	float GetFarZ() const;
	float GetAspect() const;
	float GetFovY() const;
	float GetFovX() const;

	void LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp);
	void LookAt(const DirectX::XMFLOAT3& pos, const Vector3& target, const Vector3& up);

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

	void SetOffset(float x, float y, float z);
	void Move(const Vector3& shift);
	const DirectX::BoundingFrustum& GetFrustum() const;

protected:
	Vector3 position { 0.0f, 0.0f, 0.0f };
	Vector3 right { 1.0f, 0.0f, 0.0f };
	Vector3 up { 0.0f, 1.0f, 0.0f };
	Vector3 look { 0.0f, 0.0f, 1.0f };

	Vector3 offset{ 0.0f, 0.0f, 0.0f };

	float nearZ{ 0.0f };
	float farZ{ 0.0f };
	float aspect{ 0.0f };
	float fovY{ 0.0f };

	DirectX::XMFLOAT4X4 viewMatrix{ Math::Identity4x4() };
	DirectX::XMFLOAT4X4 projMatrix{ Math::Identity4x4() };

	float mNearWindowHeight{ 0.0f };
	float mFarWindowHeight{ 0.0f };

	DirectX::BoundingFrustum frustum;
};

