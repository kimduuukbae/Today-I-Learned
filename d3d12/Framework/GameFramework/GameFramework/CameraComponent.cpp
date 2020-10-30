#include "stdafx.h"
#include "CameraComponent.h"
#include "GameplayStatics.h"

using namespace DirectX;

CameraComponent::CameraComponent()
{
	SetLens(0.25f * XM_PI, GameplayStatics::GetAspectRatio() , 1.0f, 1000.0f);
}

CameraComponent::~CameraComponent()
{
	
}

XMVECTOR CameraComponent::GetPosition()const
{
	return XMLoadFloat3(&position);
}

XMFLOAT3 CameraComponent::GetPosition3f()const
{
	return position;
}

void CameraComponent::SetPosition(float x, float y, float z)
{
	position = { x, y, z };
	UpdateMatrix();
}

void CameraComponent::SetPosition(const XMFLOAT3& v)
{
	position = v;
	UpdateMatrix();
}

XMVECTOR CameraComponent::GetRight()const
{
	return XMLoadFloat3(&right);
}

XMFLOAT3 CameraComponent::GetRight3f()const
{
	return right;
}

XMVECTOR CameraComponent::GetUp()const
{
	return XMLoadFloat3(&up);
}

XMFLOAT3 CameraComponent::GetUp3f()const
{
	return up;
}

XMVECTOR CameraComponent::GetLook()const
{
	return XMLoadFloat3(&look);
}

XMFLOAT3 CameraComponent::GetLook3f()const
{
	return look;
}

float CameraComponent::GetNearZ()const
{
	return nearZ;
}

float CameraComponent::GetFarZ()const
{
	return farZ;
}

float CameraComponent::GetAspect()const
{
	return aspect;
}

float CameraComponent::GetFovY()const
{
	return fovY;
}

float CameraComponent::GetFovX()const
{
	float halfWidth = 0.5f * GetNearWindowWidth();
	return 2.0f * atan(halfWidth / nearZ);
}

float CameraComponent::GetNearWindowWidth()const
{
	return aspect * mNearWindowHeight;
}

float CameraComponent::GetNearWindowHeight()const
{
	return mNearWindowHeight;
}

float CameraComponent::GetFarWindowWidth()const
{
	return aspect * mFarWindowHeight;
}

float CameraComponent::GetFarWindowHeight()const
{
	return mFarWindowHeight;
}

void CameraComponent::LookAt(DirectX::FXMVECTOR pos, DirectX::FXMVECTOR target, DirectX::FXMVECTOR worldUp)
{
	XMVECTOR L = XMVector3Normalize(XMVectorSubtract(target, pos));
	XMVECTOR R = XMVector3Normalize(XMVector3Cross(worldUp, L));
	XMVECTOR U = XMVector3Cross(L, R);

	XMStoreFloat3(&position, pos);
	XMStoreFloat3(&look, L);
	XMStoreFloat3(&right, R);
	XMStoreFloat3(&up, U);

	UpdateMatrix();
}

void CameraComponent::LookAt(const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& target, const DirectX::XMFLOAT3& up)
{
	XMVECTOR P = XMLoadFloat3(&pos);
	XMVECTOR T = XMLoadFloat3(&target);
	XMVECTOR U = XMLoadFloat3(&up);

	LookAt(P, T, U);
}

DirectX::XMMATRIX CameraComponent::GetView() const
{
	return XMLoadFloat4x4(&viewMatrix);
}

DirectX::XMMATRIX CameraComponent::GetProj() const
{
	return XMLoadFloat4x4(&projMatrix);
}

DirectX::XMFLOAT4X4 CameraComponent::GetView4x4f() const
{
	return viewMatrix;
}

DirectX::XMFLOAT4X4 CameraComponent::GetProj4x4f() const
{
	return projMatrix;
}

void CameraComponent::UpdateMatrix()
{
	XMVECTOR R = XMLoadFloat3(&right);
	XMVECTOR U = XMLoadFloat3(&up);
	XMVECTOR L = XMLoadFloat3(&look);
	XMVECTOR P = XMLoadFloat3(&position);

	// 카메라의 축 직교 벡터들을 단위벡터로 만듬
	L = XMVector3Normalize(L);
	U = XMVector3Normalize(XMVector3Cross(L, R));
	R = XMVector3Cross(U, L);

	// view 매트릭스의 원소들을 채움
	float x = -XMVectorGetX(XMVector3Dot(P, R));
	float y = -XMVectorGetX(XMVector3Dot(P, U));
	float z = -XMVectorGetX(XMVector3Dot(P, L));

	XMStoreFloat3(&right, R);
	XMStoreFloat3(&up, U);
	XMStoreFloat3(&look, L);

	// rX rY rZ 0
	// uX uY uZ 0
	// lX lY lZ 0
	// -x -y -z 1
	viewMatrix(0, 0) = right.x;
	viewMatrix(1, 0) = right.y;
	viewMatrix(2, 0) = right.z;
	viewMatrix(3, 0) = x;

	viewMatrix(0, 1) = up.x;
	viewMatrix(1, 1) = up.y;
	viewMatrix(2, 1) = up.z;
	viewMatrix(3, 1) = y;

	viewMatrix(0, 2) = look.x;
	viewMatrix(1, 2) = look.y;
	viewMatrix(2, 2) = look.z;
	viewMatrix(3, 2) = z;

	viewMatrix(0, 3) = 0.0f;
	viewMatrix(1, 3) = 0.0f;
	viewMatrix(2, 3) = 0.0f;
	viewMatrix(3, 3) = 1.0f;
}

void CameraComponent::SetLens(float fovY, float aspect, float nZ, float fZ)
{
	this->fovY = fovY;
	this->aspect = aspect;
	this->nearZ = nZ;
	this->farZ = fZ;

	mNearWindowHeight = 2.0f * nZ * tanf(0.5f * fovY);
	mFarWindowHeight = 2.0f *  fZ * tanf(0.5f * fovY);

	XMMATRIX P = XMMatrixPerspectiveFovLH(fovY, aspect, nZ, fZ);
	XMStoreFloat4x4(&projMatrix, P);
}

void CameraComponent::Walk(float d)
{
	// mPosition += d*mLook
	XMVECTOR s = XMVectorReplicate(d);
	XMVECTOR l = XMLoadFloat3(&look);
	XMVECTOR p = XMLoadFloat3(&position);
	XMStoreFloat3(&position, XMVectorMultiplyAdd(s, l, p));

	UpdateMatrix();
}