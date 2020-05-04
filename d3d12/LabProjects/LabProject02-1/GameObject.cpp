#include "stdafx.h"
#include "GameObject.h"

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
	Rotate(rotationSpeed.x * fElapsedTime, rotationSpeed.y * fElapsedTime, rotationSpeed.z * fElapsedTime);
}

void CGameObject::Render(HDC hDCFrameBuffer){
	if (m_pMesh) {
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
void CGameObject::SetRotation(float x, float y, float z) {
	XMMATRIX mat{ XMMatrixRotationRollPitchYaw(z, x, y) };
	::memcpy(&worldMatrix, &mat, sizeof(float) * 12);
}
void CGameObject::SetRotationSpeed(float x, float y, float z) {
	XMStoreFloat3(&rotationSpeed, XMVECTOR{ x,y,z });
}
// 객체를 x, y, z 축으로 이동
void CGameObject::Move(float x, float y, float z) {
	worldMatrix._41 += x;
	worldMatrix._42 += y;
	worldMatrix._43 += z;
}
void CGameObject::Rotate(float x, float y, float z) {
	XMFLOAT4X4& t{ worldMatrix };
	XMFLOAT3X3 f{ t._11, t._12, t._13,
				 t._21, t._22, t._23,
				 t._31, t._32, t._33 };
	XMMATRIX mat{ XMMatrixMultiply(XMLoadFloat3x3(&f), XMMatrixRotationRollPitchYaw(z, x, y)) };
	::memcpy(&worldMatrix, &mat, sizeof(float) * 12);
}

void CGameObject::SetColor(DWORD dwColor) {
	m_dwColor = dwColor;
}