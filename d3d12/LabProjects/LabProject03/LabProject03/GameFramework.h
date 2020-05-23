#pragma once
#include "Player.h"
#include "Scene.h"
#include "Timer.h"

class CGameFramework{
public:
	CGameFramework() { }
	~CGameFramework() { }

private:
	HINSTANCE m_hInstance = { nullptr };
	HWND m_hWnd = { nullptr };
	RECT m_rcClient{};
	
	HDC m_hDCFrameBuffer = { nullptr };
	HBITMAP m_hBitmapFrameBuffer = { nullptr };
	HBITMAP m_hBitmapSelect = { nullptr };

	CAirplanePlayer* m_pPlayer = { nullptr };
	CScene* m_pScene = { nullptr };
	CGameTimer m_GameTimer;
	POINT m_ptOldCursorPos;
	_TCHAR m_pszFrameRate[50];

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

	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam);
};

