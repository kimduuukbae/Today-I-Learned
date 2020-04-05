#include "stdafx.h"
#include "GameObject.h"

CGameObject::~CGameObject() {
	if (m_pMesh)
		m_pMesh->Release();
}

CPoint3D CGameObject::WorldTransfrom(CPoint3D& f3Model){
	float fPitch = degreeToRadian(m_fxRotation);
	float fYaw = degreeToRadian(m_fyRotation);
	float fRoll = degreeToRadian(m_fzRotation);

	CPoint3D f3World = f3Model;
	CPoint3D f3Rotated = f3Model;

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
	f3World.x += m_fxPosition;
	f3World.y += m_fyPosition;
	f3World.z += m_fzPosition;
	return f3World;
}

void CGameObject::Animate(float fElapsedTime){
	Rotate(m_fxRotationSpeed * fElapsedTime, m_fyRotationSpeed * fElapsedTime,
		m_fzRotationSpeed * fElapsedTime);
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
