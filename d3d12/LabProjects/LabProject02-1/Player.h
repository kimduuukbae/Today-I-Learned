#pragma once
#include "GameObject.h"
#include "Camera.h"

class CPlayer : public CGameObject{
public:
	CPlayer() : m_pCamera{ } {}
	virtual ~CPlayer() {
		if (m_pCamera)
			delete m_pCamera;
	}
private:
	CCamera* m_pCamera = { nullptr };

public:
	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void Move(float x, float y, float z);
	void Rotate(float fPitch, float fYaw, float fRoll);

	void SetCamera(CCamera* pCamera) {
		m_pCamera = pCamera;
	}
	CCamera* GetCamera() { return m_pCamera; }
};

