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
	// 메쉬의 정점 하나를 게임 객체의 위치와 방향을 사용하여 월드 좌표 변환을 함
	DirectX::XMVECTOR XM_CALLCONV WorldTransform(DirectX::FXMVECTOR& f3Model);
	// 게임 객체를 애니메이션 한다.
	virtual void Animate(float fElapsedTime);
	// 게임 객체를 렌더링한다.
	virtual void Render(HDC hDCFrameBuffer);
};

