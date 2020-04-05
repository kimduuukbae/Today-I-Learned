#pragma once
#include "Player.h"
#include "Scene.h"

class CGameFramework{
public:
	CGameFramework() { }
	~CGameFramework() { }

private:
	HINSTANCE m_hInstance = { nullptr };
	HWND m_hWnd = { nullptr };
	RECT m_rcClient;
	
	HDC m_hDCFrameBuffer = { nullptr };
	HBITMAP m_hBitmapFrameBuffer = { nullptr };
	HBITMAP m_hBitmapSelect = { nullptr };

	CPlayer* m_pPlayer = { nullptr };
	CScene* m_pScene = { nullptr };

public:
	void OnCreate(HINSTANCE hInstance, HWND hMainWnd);
	void OnDestroy();

	void BuildFrameBuffer();
	void ClearFrameBuffer(DWORD dwColor);
	void PresentFrameBuffer();

	void BuildObjects();
	void ReleaseObjects();

	void ProcessInput();
	void AnimateObjects();
	void FrameAdvance();
};

