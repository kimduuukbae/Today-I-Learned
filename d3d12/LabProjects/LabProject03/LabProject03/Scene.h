#pragma once
#include "EnemyBox.h"
#include "Camera.h"
#include "Player.h"

class CScene{
public:
	CScene(CAirplanePlayer* pPlayer) { m_pPlayer = pPlayer; }
	virtual ~CScene() {}

private:
	int m_nObjects { 0 };
	int m_nMapObjects{ 30 };
	CEnemyBox** m_ppObjects { nullptr };
	CAirplanePlayer* m_pPlayer;
	CGameObject* m_pMapObjects;

	CEnemyBox* isPickingObject{ nullptr };

public:
	virtual void BuildObjects();
	virtual void ReleaseObjects();

	virtual void Animate(float fElapsedTime);
	virtual void Render(HDC hDCFrameBuffer, CCamera* pCamera);

	virtual void OnProcessingMouseMesseage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {}
	virtual void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {}

	virtual int checkObjects(XMFLOAT3& rayOrigin, XMFLOAT3& rayDir);

	CGameObject* GetGameObject(int index);
	void ProcessCollision();
};

