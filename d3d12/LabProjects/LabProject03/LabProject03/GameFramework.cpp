#include "stdafx.h"
#include "GameFramework.h"
#include "GraphicsPipeline.h"
#include <iostream>
using namespace DirectX;

void CGameFramework::OnCreate(HINSTANCE hInstance, HWND hMainWnd) {
	::srand(timeGetTime());
	
	m_hInstance = hInstance;
	m_hWnd = hMainWnd;
	BuildFrameBuffer();
	BuildObjects();

	_tcscpy_s(m_pszFrameRate, _T("LabProject ("));
}

void CGameFramework::OnDestroy() {
	ReleaseObjects();
	if (m_hBitmapFrameBuffer) ::DeleteObject(m_hBitmapFrameBuffer);
	if (m_hDCFrameBuffer) ::DeleteDC(m_hDCFrameBuffer);
}

void CGameFramework::BuildFrameBuffer() {
	::GetClientRect(m_hWnd, &m_rcClient);
	HDC hDC = ::GetDC(m_hWnd);
	m_hDCFrameBuffer = ::CreateCompatibleDC(hDC);
	m_hBitmapFrameBuffer = ::CreateCompatibleBitmap(hDC,
		m_rcClient.right - m_rcClient.left,
		m_rcClient.bottom - m_rcClient.top);
	::SelectObject(m_hDCFrameBuffer, m_hBitmapFrameBuffer);
	::ReleaseDC(m_hWnd, hDC);
	::SetBkMode(m_hDCFrameBuffer, TRANSPARENT);
}

void CGameFramework::ClearFrameBuffer(DWORD dwColor) {
	HPEN hPen = ::CreatePen(PS_SOLID, 0, dwColor);
	HPEN hOldPen = (HPEN)::SelectObject(m_hDCFrameBuffer, hPen);
	HBRUSH hBrush = ::CreateSolidBrush(dwColor);
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(m_hDCFrameBuffer, hBrush);
	::Rectangle(m_hDCFrameBuffer, m_rcClient.left, m_rcClient.top
		, m_rcClient.right, m_rcClient.bottom);
	::SelectObject(m_hDCFrameBuffer, hOldBrush);
	::SelectObject(m_hDCFrameBuffer, hOldPen);
	::DeleteObject(hPen);
	::DeleteObject(hBrush);
}

void CGameFramework::PresentFrameBuffer() {
	HDC hDC = ::GetDC(m_hWnd);
	::BitBlt(hDC, m_rcClient.left, m_rcClient.top, m_rcClient.right
		- m_rcClient.left, m_rcClient.bottom - m_rcClient.top,
		m_hDCFrameBuffer, m_rcClient.left, m_rcClient.top, SRCCOPY);
	::ReleaseDC(m_hWnd, hDC);
}

void CGameFramework::BuildObjects() {
	CCamera* pCamera = new CCamera();
	pCamera->SetViewport(0, 0, FRAMEBUFFER_WIDTH, FRAMEBUFFER_HEIGHT);
	pCamera->GeneratePerspectiveProjectionMatrix(1.01f, 500.0f, 60.0f);
	pCamera->SetFOVAngle(60.0f);

	CAirplaneMesh* pAirplaneMesh = new CAirplaneMesh{ 6.0f, 6.0f, 1.0f };

	m_pPlayer = new CAirplanePlayer();
	m_pPlayer->SetPosition(0.0f, 0.0f, 0.0f);
	m_pPlayer->SetMesh(pAirplaneMesh);
	m_pPlayer->SetColor(RGB(0, 0, 255));
	m_pPlayer->SetCamera(pCamera);
	m_pPlayer->SetCameraOffset(XMFLOAT3{ 0.0f, 5.0f, -15.0f });

	m_pScene = new CScene(m_pPlayer);
	m_pScene->BuildObjects();
}

void CGameFramework::ReleaseObjects() {
	if (m_pScene) {
		m_pScene->ReleaseObjects();
		delete m_pScene;
	}
	if (m_pPlayer) delete m_pPlayer;
}

void CGameFramework::ProcessInput() {

	static UCHAR pKeyBuffer[256];
	if (::GetKeyboardState(pKeyBuffer)) {
		DWORD dwDirection{ 0 };
		if (pKeyBuffer[VK_UP] & 0xF0) dwDirection |= DIR_FORWARD;
		if (pKeyBuffer[VK_DOWN] & 0xF0) dwDirection |= DIR_BACKWARD;
		if (pKeyBuffer[VK_LEFT] & 0xF0) dwDirection |= DIR_LEFT;
		if (pKeyBuffer[VK_RIGHT] & 0xF0) dwDirection |= DIR_RIGHT;
		if (pKeyBuffer[VK_PRIOR] & 0xF0) dwDirection |= DIR_UP;
		if (pKeyBuffer[VK_NEXT] & 0xF0) dwDirection |= DIR_DOWN;
		if (dwDirection) m_pPlayer->Move(dwDirection, 0.15f);
	}
	if (::GetCapture() == m_hWnd) {
		::SetCursor(NULL);
		POINT ptCursorPos;
		::GetCursorPos(&ptCursorPos);
		POINT ptPickingPos = ptCursorPos;
		float cxMouseDelta =
			static_cast<float>(ptCursorPos.x - m_ptOldCursorPos.x) / 3.0f;
		float cyMouseDelta =
			static_cast<float>(ptCursorPos.y - m_ptOldCursorPos.y) / 3.0f;


		// ��ŷ = ������ Ŭ���� �ϸ� ��ũ�� ��ǥ�� �Ѿ��
		// ����' = ���� * ���� * �� * ���� + ��ũ�� = ��ü
		// ���콺�� ��ũ�� ��ǥ���� ���÷� �����Ű��
		// �� �� �̻� �浹�Ǵ� ������ �����, ��ũ�� ��ǥ���� üũ�ϸ� ���������� �浹��
		// ���ñ��� ���� z Ȯ������ �ʰ�, ���忡�� z ó��
		if (pKeyBuffer[VK_RBUTTON] & 0xF0) {
			ScreenToClient(m_hWnd, &ptPickingPos);
			// ���������� Ŭ���� ��ǥ�� Ŭ���̾�Ʈ ��ǥ�� ����

			CViewport& viewport{ m_pPlayer->GetCamera()->m_Viewport };
			CCamera& cam{ *m_pPlayer->GetCamera() };

			// ȭ�� ��ǥ�� -> ���� ��ǥ
			float xProjectPos{ 2.0f * ptPickingPos.x / (float)viewport.m_nWidth - 1.0f  };
			float yProjectPos{ -2.0f * ptPickingPos.y / (float)viewport.m_nHeight + 1.0f  };
			
			// ���� ��ǥ -> ī�޶� ��ǥ
			// ���� ��ǥ������ �浹 ó���� �� �� ������ �ֵ��� ��ĥ �� ����
			// ���� ���� ��ǥ ��Ŀ��ٰ� ������ǥ ���� ������ ī�޶� ��ǥ�� ����
			// ������ pdf �� ��������.

			float xCamPos{ xProjectPos / cam.projectMatrix._11};
			float yCamPos{ yProjectPos / cam.projectMatrix._22 };
			float zCamPos{ 1.0f };

			XMFLOAT4X4 viewInverse{};
			XMStoreFloat4x4(&viewInverse, XMMatrixInverse(nullptr, XMLoadFloat4x4(&cam.viewMatrix)));
			
			// ī�޶� ��ǥ ��� ����ķ� �������� ���� ��ǥ�� ����
			// ī�޶� ��ǥ ��� ����� -> ���� ��ǥ ���
			XMFLOAT3 rayDir{xCamPos, yCamPos, zCamPos}, rayDirOrigin{};
			XMStoreFloat3(&rayDirOrigin, 
				XMVector3TransformCoord(XMLoadFloat3(&rayDirOrigin), XMLoadFloat4x4(&viewInverse)));
			XMStoreFloat3(&rayDir,
				XMVector3TransformNormal(XMLoadFloat3(&rayDir), XMLoadFloat4x4(&viewInverse)));

			// ��� ������Ʈ�鿡 ���ؼ� ���� ���� ��ŷó�� �غ�
			// ��ŷ�ߴµ� ��ŷ�� ��ü�� �ִٸ� player���� �Ѱ���
			if (int idx{ m_pScene->checkObjects(rayDirOrigin, rayDir) };idx != -1)
				m_pPlayer->SetPickingObject(m_pScene->GetGameObject(idx));
		}

		::SetCursorPos(m_ptOldCursorPos.x, m_ptOldCursorPos.y);
		if (cxMouseDelta || cyMouseDelta) {
			if (pKeyBuffer[VK_RBUTTON] & 0xF0)
				m_pPlayer->Rotate(cyMouseDelta, 0.0f, -cxMouseDelta);
			else
				m_pPlayer->Rotate(cyMouseDelta, cxMouseDelta, 0.0f);
		}
	}
	m_pPlayer->Update(m_GameTimer.GetTimeElapsed());
}

void CGameFramework::AnimateObjects() {
	float fTimeElapsed{ m_GameTimer.GetTimeElapsed() };
	if (m_pPlayer) {
		m_pPlayer->Animate(fTimeElapsed);
		for (auto& bullet : m_pPlayer->GetBullets())
			bullet->Animate(fTimeElapsed);
	}
	if (m_pScene) m_pScene->Animate(fTimeElapsed);
}

void CGameFramework::FrameAdvance() {

	m_GameTimer.Tick(60.0f);

	ProcessInput();
	AnimateObjects();

	ClearFrameBuffer(RGB(255,255,255));

	CCamera* pCamera = m_pPlayer->GetCamera();
	if (m_pScene) {
		m_pScene->Render(m_hDCFrameBuffer, pCamera);
		m_pScene->ProcessCollision();
	}

	if (m_pPlayer) {
		m_pPlayer->Render(m_hDCFrameBuffer, pCamera);
		for (auto& bullet : m_pPlayer->GetBullets())
			bullet->Render(m_hDCFrameBuffer, pCamera);
		
	}


	PresentFrameBuffer();

	m_GameTimer.GetFrameRate(m_pszFrameRate + 12, 37);
	::SetWindowText(m_hWnd, m_pszFrameRate);
}

void CGameFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	switch (nMessageID) {
		
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDOWN:
		::SetCapture(hWnd);
		::GetCursorPos(&m_ptOldCursorPos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		::ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		if (m_pScene) m_pScene->OnProcessingMouseMesseage(hWnd, nMessageID, wParam, lParam);
		break;
	}
}

void CGameFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	switch (nMessageID) {
	case WM_KEYDOWN:
		switch (wParam) {
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		case VK_RETURN:
			break;
		case VK_CONTROL:
			m_pPlayer->ShootBullet();
			break;
		default:
			if (m_pScene) m_pScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
			break;
		}
		break;
	default:
		break;
	}
}

LRESULT CALLBACK CGameFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	switch (nMessageID) {
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return 0;
}