#pragma once
#include "GameObject.h"
#include "Player.h"
class CScene{
public:
	CScene() {}
	virtual ~CScene() {}

private:
	int m_nObjects = { 0 };
	CGameObject** m_ppObjects = { nullptr };

public:
	virtual void BuildObjects();
	virtual void ReleaseObjects();

	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, CCamera* pCamera);
};

