#pragma once
#include "Mesh.h"
class CGameObject
{
public:
	CGameObject() {}
	~CGameObject();

private:
	// 객체의 월드 좌표계
	DirectX::XMFLOAT3 position{};
	// 객체의 x, y, z 축 회전 양 (축을 기준으로 반시계 방향)
	DirectX::XMFLOAT3 rotation{};
	// 객체의 x, y, z 축 회전 양
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
	// 메쉬의 정점 하나를 게임 객체의 위치와 방향을 사용하여 월드 좌표 변환을 함
	DirectX::XMVECTOR XM_CALLCONV WorldTransfrom(DirectX::FXMVECTOR& f3Model);
	// 게임 객체를 애니메이션 한다.
	virtual void Animate(float fElapsedTime);
	// 게임 객체를 렌더링한다.
	virtual void Render(HDC hDCFrameBuffer);
};

