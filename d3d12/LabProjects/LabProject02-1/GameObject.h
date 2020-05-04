#pragma once
#include "Mesh.h"
class CGameObject
{
public:
	CGameObject() {}
	~CGameObject();

private:
	DirectX::XMFLOAT4X4 worldMatrix{
		1.0f,0.0f,0.0f,0.0f,
		0.0f,1.0f,0.0f,0.0f,
		0.0f,0.0f,1.0f,0.0f,
		0.0f,0.0f,0.0f,1.0f
	}; // right, up, look, position

	DirectX::XMFLOAT3 rotationSpeed;

	CMesh* m_pMesh { nullptr };
	DWORD m_dwColor { RGB(255,0,0) };
	

public:
	void SetMesh(CMesh* pMesh);
	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void SetRotationSpeed(float x, float y, float z);
	void Move(float x, float y, float z);
	void Rotate(float x, float y, float z);
	void SetColor(DWORD dwColor);
public:
	// �޽��� ���� �ϳ��� ���� ��ü�� ��ġ�� ������ ����Ͽ� ���� ��ǥ ��ȯ�� ��
	DirectX::XMVECTOR XM_CALLCONV WorldTransform(DirectX::FXMVECTOR& f3Model);
	// ���� ��ü�� �ִϸ��̼� �Ѵ�.
	virtual void Animate(float fElapsedTime);
	// ���� ��ü�� �������Ѵ�.
	virtual void Render(HDC hDCFrameBuffer);
};

