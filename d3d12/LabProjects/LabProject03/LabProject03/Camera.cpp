#include "stdafx.h"
#include "Camera.h"
#include "Player.h"

using namespace DirectX;

CCamera::~CCamera() {}

void CCamera::GenerateViewMatrix(){
	XMVECTOR tLook{ XMVector3Normalize(XMLoadFloat3(&look)) };
	XMVECTOR tUp{ XMVector3Normalize(XMLoadFloat3(&up)) };
	XMVECTOR tRight{ XMVector3Normalize(XMVector3Cross(tUp, tLook)) };

	tUp = XMVector3Normalize(XMVector3Cross(tLook, tRight));

	XMStoreFloat3(&look, tLook);
	XMStoreFloat3(&up, tUp);
	XMStoreFloat3(&right, tRight);


	// 카메라 변환 행렬 = 카메라 월드 변환 행렬의 역행렬
	viewMatrix._11 = right.x;
	viewMatrix._12 = up.x;
	viewMatrix._13 = look.x;
	viewMatrix._21 = right.y;
	viewMatrix._22 = up.y;
	viewMatrix._23 = look.y;
	viewMatrix._31 = right.z;
	viewMatrix._32 = up.z;
	viewMatrix._33 = look.z;

	XMVECTOR tPos{ XMLoadFloat3(&position) };
	viewMatrix._41 = -XMVectorGetX(XMVector3Dot(tPos, tRight));
	viewMatrix._42 = -XMVectorGetX(XMVector3Dot(tPos, tUp));
	viewMatrix._43 = -XMVectorGetX(XMVector3Dot(tPos, tLook));

	XMStoreFloat4x4(&viewProjectMatrix, XMMatrixMultiply
	(XMLoadFloat4x4(&viewMatrix), XMLoadFloat4x4(&projectMatrix)));
}

void CCamera::GeneratePerspectiveProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fFovAngle){
	// 원근 투영 변환 행렬 생성
	float fAspectRatio{ static_cast<float>(m_Viewport.m_nWidth) / static_cast<float>(m_Viewport.m_nHeight) };
	XMStoreFloat4x4(&projectMatrix,
	XMMatrixPerspectiveFovLH(XMConvertToRadians(fFovAngle), fAspectRatio,
	fNearPlaneDistance, fFarPlaneDistance));
}

void CCamera::SetViewport(int nLeft, int nTop, int nWidth, int nHeight){
	m_Viewport.SetViewport(nLeft, nTop, nWidth, nHeight);
	m_fAspectRatio = static_cast<float>(nWidth) / static_cast<float>(nHeight);
}

void CCamera::SetFOVAngle(float fFOVAngle){
	m_fFOVAngle = fFOVAngle;
	m_fProjectRectDistance = 1.0f / std::tanf(XMConvertToRadians(fFOVAngle * 0.5f));
}

void CCamera::SetLookAt(DirectX::XMFLOAT3& lookAt, DirectX::XMFLOAT3& up){
	// 현재 카메라의 위치에서 플레이어 보는 방향으로 변환 행렬 생성
	SetLookAt(position, lookAt, up);
}

void CCamera::SetLookAt(DirectX::XMFLOAT3& pos, DirectX::XMFLOAT3& lookAt, DirectX::XMFLOAT3& up){
	position = pos;
	XMStoreFloat4x4(&viewMatrix,
	XMMatrixLookAtLH(XMLoadFloat3(&position),
	XMLoadFloat3(&lookAt), XMLoadFloat3(&up)));

	// 카메라 변환행렬에서 x, y, z 축 구하기
	XMVECTORF32 f32Right{ viewMatrix._11,viewMatrix._21,viewMatrix._31 };
	XMVECTORF32 f32Up{ viewMatrix._12,viewMatrix._22,viewMatrix._32 };
	XMVECTORF32 f32Look{ viewMatrix._13,viewMatrix._23,viewMatrix._33 };

	XMStoreFloat3(&right, XMVector3Normalize(f32Right));
	XMStoreFloat3(&this->up, XMVector3Normalize(f32Up));
	XMStoreFloat3(&look, XMVector3Normalize(f32Look));
}

void CCamera::Move(float x, float y, float z){
	XMFLOAT3 tMove{ x,y,z };
	Move(tMove);
}

void CCamera::Move(DirectX::XMFLOAT3& shift){
	XMStoreFloat3(&position, 
		XMVectorAdd(XMLoadFloat3(&position), XMLoadFloat3(&shift)));
}

void CCamera::Rotate(float fPitch, float fYaw, float fRoll){
	if (fPitch != 0.0f) {
		XMMATRIX tRotate{ XMMatrixRotationAxis(XMLoadFloat3(&right), XMConvertToRadians(fPitch)) };
		XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), tRotate));
		XMStoreFloat3(&up, XMVector3TransformNormal(XMLoadFloat3(&up), tRotate));
	}
	if (fYaw != 0.0f) {
		XMMATRIX tRotate{ XMMatrixRotationAxis(XMLoadFloat3(&up), XMConvertToRadians(fYaw)) };
		XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), tRotate));
		XMStoreFloat3(&right, XMVector3TransformNormal(XMLoadFloat3(&right), tRotate));
	}
	if (fRoll != 0.0f) {
		XMMATRIX tRotate{ XMMatrixRotationAxis(XMLoadFloat3(&look), XMConvertToRadians(fRoll)) };
		XMStoreFloat3(&up, XMVector3TransformNormal(XMLoadFloat3(&up), tRotate));
		XMStoreFloat3(&right, XMVector3TransformNormal(XMLoadFloat3(&right), tRotate));
	}
}

void CCamera::Update(CPlayer* pPlayer, DirectX::XMFLOAT3& lookAt, float fElapsedTime){
	// 플레이어의 로컬 x-축, y-축 , z-축 벡터로부터 회전행렬 (플레이어와 같은 방향)을 생성
	XMMATRIX tRotate;
	tRotate.r[0] = XMVectorSet(pPlayer->right.x, pPlayer->right.y, pPlayer->right.z, 0.0f);
	tRotate.r[1] = XMVectorSet(pPlayer->up.x, pPlayer->up.y, pPlayer->up.z, 0.0f);
	tRotate.r[2] = XMVectorSet(pPlayer->look.x, pPlayer->look.y, pPlayer->look.z, 0.0f);
	tRotate.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);

	XMVECTOR tPos{ XMLoadFloat3(&position) };
	XMVECTOR tOffset{ XMVector3TransformCoord(XMLoadFloat3(&pPlayer->cameraOffset), tRotate) };

	// 회전한 카메라의 위치는 플레이어 위치에 회전한 카메라 오프셋 벡터를 더한 것
	XMVECTOR tNewPos{ XMVectorAdd(XMLoadFloat3(&pPlayer->position), tOffset) };
	// 현재의 카메라 위치에서 회전한 카메라의 위치까지의 방향과 거리를 나타내는 벡터
	XMVECTOR tDir{ XMVectorSubtract(tNewPos, tPos) };
	float fLength{ XMVectorGetX(XMVector3Length(tDir)) };
	tDir = XMVector3Normalize(tDir);

	//lag 
	float fTimeLagScale{ fElapsedTime * 4.0f };
	float fDistance{ fLength * fTimeLagScale };
	if (fDistance > fLength) fDistance = fLength;
	if (fLength < 0.01f) fDistance = fLength;
	if (fDistance > 0) {
		XMStoreFloat3(&position, XMVectorAdd(tPos, XMVectorScale(tDir, fDistance)));
		SetLookAt(pPlayer->position, pPlayer->up);
	}
}

void CCamera::SetPosition(float x, float y, float z) {
	viewMatrix._41 = x;
	viewMatrix._42 = y;
	viewMatrix._43 = z;
}

void CCamera::SetRotation(float fPitch, float fYaw, float fRoll) {
	XMMATRIX mat{ XMMatrixRotationRollPitchYaw(fRoll, fPitch, fYaw) };
	::memcpy(&viewMatrix, &mat, sizeof(float) * 12);
}

void CViewport::SetViewport(int nLeft, int nTop, int nWidth, int nHeight){
	m_nLeft = nLeft;
	m_nTop = nTop;
	m_nWidth = nWidth;
	m_nHeight = nHeight;
}
