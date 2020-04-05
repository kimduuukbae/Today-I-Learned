#pragma once
#include "Mesh.h"
class CGameObject
{
public:
	CGameObject() {}
	~CGameObject();

private:
	// ��ü�� ���� ��ǥ��
	float m_fxPosition = { 0.0f };
	float m_fyPosition = { 0.0f };
	float m_fzPosition = { 0.0f };
	// ��ü�� x, y, z �� ȸ�� �� (���� �������� �ݽð� ����)
	float m_fxRotation = { 0.0f };
	float m_fyRotation = { 0.0f };
	float m_fzRotation = { 0.0f };
	// ��ü�� x, y, z �� ȸ�� ��
	float m_fxRotationSpeed = { 0.0f };
	float m_fyRotationSpeed = { 0.0f };
	float m_fzRotationSpeed = { 0.0f };

	CMesh* m_pMesh = { nullptr };
	DWORD m_dwColor = { RGB(255,0,0) };

public:
	void SetMesh(CMesh* pMesh) {
		m_pMesh = pMesh;
		if (pMesh)
			pMesh->AddRef();
	}
	void SetColor(DWORD dwColor) { m_dwColor = dwColor; }
	void SetPosition(float x, float y, float z) {
		m_fxPosition = x;
		m_fyPosition = y;
		m_fzPosition = z;
	}
	void SetRotation(float x, float y, float z) {
		m_fxRotation = x;
		m_fyRotation = y;
		m_fzRotation = z;
	}
	void SetRotationSpeed(float x, float y, float z) {
		m_fxRotationSpeed = x;
		m_fyRotationSpeed = y;
		m_fzRotationSpeed = z;
	}
	// ��ü�� x, y, z ������ �̵�
	void Move(float x, float y, float z) {
		m_fxPosition += x;
		m_fyPosition += y;
		m_fzPosition += z;
	}
	void Rotate(float x, float y, float z) {
		m_fxRotation += x;
		m_fyRotation += y;
		m_fzRotation += z;
	}
public:
	// �޽��� ���� �ϳ��� ���� ��ü�� ��ġ�� ������ ����Ͽ� ���� ��ǥ ��ȯ�� ��
	CPoint3D WorldTransfrom(CPoint3D& f3Model);
	// ���� ��ü�� �ִϸ��̼� �Ѵ�.
	virtual void Animate(float fElapsedTime);
	// ���� ��ü�� �������Ѵ�.
	virtual void Render(HDC hDCFrameBuffer);
};

