#include "stdafx.h"
#include "Camera.h"
#include "GameObject.h"
#include "GraphicsPipeline.h"

using namespace DirectX;

CGameObject::~CGameObject() {
	if (m_pMesh)
		m_pMesh->Release();
}

void CGameObject::SetMesh(CMesh* pMesh) {
	m_pMesh = pMesh;
	if (pMesh)
		pMesh->AddRef();
}

XMVECTOR CGameObject::WorldTransform(FXMVECTOR& f3Model){
	
	return XMVector3TransformCoord(f3Model, XMLoadFloat4x4(&worldMatrix));
}

void CGameObject::Animate(float fElapsedTime){
	if (rotationSpeed != 0.0f)
		Rotate(rotationAxis, rotationSpeed * fElapsedTime);
	if (speed != 0.0f)
		Move(direction, speed * fElapsedTime);

}

void CGameObject::Render(HDC hDCFrameBuffer, CCamera* pCamera){
	if (m_pMesh) {
		CGraphicsPipeline::SetWorldTransform(&worldMatrix);

		HPEN hPen = ::CreatePen(PS_SOLID, 0, m_dwColor);
		HPEN hOldPen = (HPEN)::SelectObject(hDCFrameBuffer, hPen);
		m_pMesh->Render(hDCFrameBuffer);
		::SelectObject(hDCFrameBuffer, hOldPen);
		::DeleteObject(hPen);
	}
}

void CGameObject::SetPosition(float x, float y, float z) {
	worldMatrix._41 = x;
	worldMatrix._42 = y;
	worldMatrix._43 = z;
}

void CGameObject::SetPosition(DirectX::XMFLOAT3& pos){
	worldMatrix._41 = pos.x;
	worldMatrix._42 = pos.y;
	worldMatrix._43 = pos.z;
}

void CGameObject::SetMovingDirection(const DirectX::XMFLOAT3& dir){
	XMStoreFloat3(&direction, XMVector3Normalize(XMLoadFloat3(&dir)));
}

void CGameObject::SetRotationAxis(const DirectX::XMFLOAT3& axis){
	XMStoreFloat3(&rotationAxis, XMVector3Normalize(XMLoadFloat3(&axis)));
}


//void CGameObject::SetRotation(float x, float y, float z) {
//	XMMATRIX mat{ XMMatrixRotationRollPitchYaw(z, x, y) };
//	::memcpy(&worldMatrix, &mat, sizeof(float) * 12);
//}
//void CGameObject::SetRotationSpeed(float x, float y, float z) {
//	XMStoreFloat3(&rotationSpeed, XMVECTOR{ x,y,z });
//}
// 객체를 x, y, z 축으로 이동
void CGameObject::Move(XMFLOAT3& dir, float fSpeed) {
	SetPosition(worldMatrix._41 + dir.x * fSpeed,
		worldMatrix._42 + dir.y * fSpeed,
		worldMatrix._43 + dir.z * fSpeed
	);
}
void CGameObject::Rotate(float fPitch, float fYaw, float fRoll) {
	XMMATRIX rot{ XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(fPitch),
		XMConvertToRadians(fYaw),
		XMConvertToRadians(fRoll)
	) };
	XMStoreFloat4x4(&worldMatrix, XMMatrixMultiply(
		rot, XMLoadFloat4x4(&worldMatrix)));

}

void CGameObject::Rotate(DirectX::XMFLOAT3& axis, float fAngle){
	XMMATRIX rot{ XMMatrixRotationAxis(
		XMLoadFloat3(&axis), XMConvertToRadians(fAngle)) };
	XMStoreFloat4x4(&worldMatrix, XMMatrixMultiply(
		rot, XMLoadFloat4x4(&worldMatrix)));
}

void CGameObject::SetColor(DWORD dwColor) {
	m_dwColor = dwColor;
}