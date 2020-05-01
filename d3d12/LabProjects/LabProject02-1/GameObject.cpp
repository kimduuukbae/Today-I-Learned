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

XMVECTOR CGameObject::WorldTransfrom(FXMVECTOR& f3Model){

	float fPitch{ degreeToRadian(rotation.x) };
	float fYaw {degreeToRadian(rotation.y)};
	float fRoll{ degreeToRadian(rotation.z) };

	XMFLOAT3 f3World{}, f3Rotated{};
	XMStoreFloat3(&f3World, f3Model);
	XMStoreFloat3(&f3Rotated, f3Model);

	if (fPitch != 0.0f) {
		f3Rotated.y = f3World.y * std::cosf(fPitch) - f3World.z * std::sinf(fPitch);
		f3Rotated.z = f3World.y * std::sinf(fPitch) + f3World.z * std::cosf(fPitch);
		f3World.y = f3Rotated.y;
		f3World.z = f3Rotated.z;
	}
	if (fYaw != 0.0f) {
		f3Rotated.x = f3World.x * std::cosf(fYaw) + f3World.z * std::sinf(fYaw);
		f3Rotated.z = -f3World.x * std::sinf(fYaw) + f3World.z * std::cosf(fYaw);
		f3World.x = f3Rotated.x;
		f3World.z = f3Rotated.z;
	}
	if (fRoll != 0.0f) {
		f3Rotated.x = f3World.x * std::cosf(fRoll) - f3World.y * std::sinf(fRoll);
		f3Rotated.y = f3World.x * std::sinf(fRoll) + f3World.y * std::cosf(fRoll);
		f3World.x = f3Rotated.x;
		f3World.y = f3Rotated.y;
	}

	f3World.x += position.x;
	f3World.y += position.y;
	f3World.z += position.z;
	return XMLoadFloat3(&f3World);
}

void CGameObject::Animate(float fElapsedTime){
	Rotate(speed.x * fElapsedTime, speed.y * fElapsedTime, speed.z * fElapsedTime);
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
	XMStoreFloat3(&position, XMVECTOR{x,y,z});
}
void CGameObject::SetRotation(float x, float y, float z) {
	XMStoreFloat3(&rotation, XMVECTOR{ x,y,z });
}
void CGameObject::SetRotationSpeed(float x, float y, float z) {
	XMStoreFloat3(&speed, XMVECTOR{ x,y,z });
}
// 객체를 x, y, z 축으로 이동
void CGameObject::Move(float x, float y, float z) {
	XMVECTOR v{ XMLoadFloat3(&position) };
	XMStoreFloat3(&position, XMVectorAdd(v, XMVECTOR{ x,y,z }));
}
void CGameObject::Rotate(float x, float y, float z) {
	XMVECTOR v{ XMLoadFloat3(&rotation) };
	XMStoreFloat3(&rotation, XMVectorAdd(v, XMVECTOR{ x,y,z }));
}

void CGameObject::SetColor(DWORD dwColor) {
	m_dwColor = dwColor;
}