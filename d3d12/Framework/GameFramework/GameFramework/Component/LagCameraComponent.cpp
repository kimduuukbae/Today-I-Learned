#include "stdafx.h"
#include "LagCameraComponent.h"
#include "Object/Object.h"

using namespace DirectX;

LagCameraComponent::LagCameraComponent() {}
LagCameraComponent::~LagCameraComponent() {}

void LagCameraComponent::Update(const GameTimer& gt)
{
	XMFLOAT3 right{ owner->GetTransform()->GetRight() };
	XMFLOAT3 up{ owner->GetTransform()->GetUp() };
	XMFLOAT3 look{ owner->GetTransform()->GetLook() };

	XMFLOAT4X4 rot
	{ 
		right.x, right.y, right.z, 0.0f,
		up.x, up.y, up.z, 0.0f,
		look.x, look.y, look.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	XMFLOAT3 ownerPos{ owner->GetTransform()->GetPosition() };

	XMFLOAT3 offset{ Math::TransformCoord(this->offset, rot) };
	XMFLOAT3 tPos{ Math::Add(ownerPos, offset) };
	XMFLOAT3 dir{ Math::Subtract(tPos, position) };
	float length{ Math::Length(dir) };
	float timeLag{ gt.DeltaTime() * (1.0f / lagScale) };
	float distance{ length * timeLag };

	if (distance > length) distance = length;
	if (length < 0.01f) distance = length;

	if (distance > 0) {
		dir = Math::Vector3Normalize(dir);
		position = Math::Add(GetPosition3f(), dir, distance);
		SetLookAt(ownerPos);
	}
	SetLookAt(ownerPos);
}

void LagCameraComponent::SetLagScale(float scale)
{
	lagScale = scale;
}

void LagCameraComponent::SetLookAt(const DirectX::XMFLOAT3& lookAt)
{
	XMFLOAT4X4 mtxLookAt{ Math::LookAtLH(position, lookAt, owner->GetTransform()->GetUp()) };
	right = XMFLOAT3(mtxLookAt._11, mtxLookAt._21, mtxLookAt._31);
	up = XMFLOAT3(mtxLookAt._12, mtxLookAt._22, mtxLookAt._32);
	look = XMFLOAT3(mtxLookAt._13, mtxLookAt._23, mtxLookAt._33);

	UpdateMatrix();
}
