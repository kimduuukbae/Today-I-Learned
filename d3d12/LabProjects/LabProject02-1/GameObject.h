#pragma once
#include "Mesh.h"
class CGameObject
{
public:
	CGameObject() {}
	~CGameObject();

private:
	// ��ü�� ���� ��ǥ��
	DirectX::XMFLOAT3 position{};
	// ��ü�� x, y, z �� ȸ�� �� (���� �������� �ݽð� ����)
	DirectX::XMFLOAT3 rotation{};
	// ��ü�� x, y, z �� ȸ�� ��
	DirectX::XMFLOAT3 speed{};

	CMesh* m_pMesh = { nullptr };
	DWORD m_dwColor = { RGB(255,0,0) };

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
	DirectX::XMVECTOR XM_CALLCONV WorldTransfrom(DirectX::FXMVECTOR& f3Model);
	// ���� ��ü�� �ִϸ��̼� �Ѵ�.
	virtual void Animate(float fElapsedTime);
	// ���� ��ü�� �������Ѵ�.
	virtual void Render(HDC hDCFrameBuffer);
};

