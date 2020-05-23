#pragma once
#include "Mesh.h"
class CCamera;

class CGameObject
{
public:
	CGameObject() {}
	~CGameObject();

	DirectX::XMFLOAT4X4 worldMatrix{ Matrix4x4::identity() };
protected:
	
	DirectX::XMFLOAT3 direction{ 0.0f, 0.0f, 1.0f };
	DirectX::XMFLOAT3 rotationAxis{ 0.0f, 1.0f, 0.0f };

	float speed{ 0.0f };
	float moveRange{ 0.0f };
	float rotationSpeed{ 0.0f };

	CMesh* m_pMesh { nullptr };
	DWORD m_dwColor { RGB(255,0,0) };
	

public:
	void SetMesh(CMesh* pMesh);

	void SetPosition(float x, float y, float z);
	constexpr XMFLOAT3 GetPosition() {
		return XMFLOAT3{ worldMatrix._41, worldMatrix._42, worldMatrix._43 };
	}
	void SetPosition(const DirectX::XMFLOAT3& pos);

	void SetMovingDirection(const DirectX::XMFLOAT3& dir);
	void SetMovingSpeed(float fSpeed) { speed = fSpeed; }
	void SetMovingRange(float fRange) { speed = fRange; }

	void SetRotationAxis(const DirectX::XMFLOAT3& axis);
	void SetRotationSpeed(float fSpeed) { rotationSpeed = fSpeed; }

	void Move(DirectX::XMFLOAT3& dir, float fSpeed);
	// 왼손좌표계에서 자전행렬은 평행이동 행렬 왼쪽에 곱한다.
	void Rotate(float fPitch, float fYaw, float fRoll);
	void Rotate(DirectX::XMFLOAT3& axis, float fAngle);

	void SetColor(DWORD dwColor);

	constexpr XMFLOAT3 GetDirection() {
		return direction;
	}
public:
	virtual void OnUpdateTransform() {}
	// 메쉬의 정점 하나를 게임 객체의 위치와 방향을 사용하여 월드 좌표 변환을 함
	DirectX::XMVECTOR XM_CALLCONV WorldTransform(DirectX::FXMVECTOR& f3Model);
	// 게임 객체를 애니메이션 한다.
	virtual void Animate(float fElapsedTime);
	// 게임 객체를 렌더링한다.
	virtual void Render(HDC hDCFrameBuffer, CCamera* pCamera);
};

