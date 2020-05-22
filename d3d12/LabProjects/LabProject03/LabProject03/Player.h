#pragma once
#include "GameObject.h"
#include "Camera.h"

class CPlayer : public CGameObject{
public:
	CPlayer() : m_pCamera{ } {}
	virtual ~CPlayer() {
		if (m_pCamera)
			delete m_pCamera;
	}
private:
	CCamera* m_pCamera { nullptr };

public:
	DirectX::XMFLOAT3 position{ 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 right{ 1.0f,0.0f,0.0f };
	DirectX::XMFLOAT3 up{ 0.0f,1.0f,0.0f };
	DirectX::XMFLOAT3 look{ 0.0f,0.0f,1.0f };

	DirectX::XMFLOAT3 cameraOffset{};
	DirectX::XMFLOAT3 velocity{};

	float m_fFriction{ 125.0f };

	float m_fPitch{};
	float m_fYaw{};
	float m_fRoll{};

	void SetPosition(float x, float y, float z);
	void SetRotation(float x, float y, float z);

	void LookAt(DirectX::XMFLOAT3& LookAt, DirectX::XMFLOAT3& up);
	void Move(DWORD dwDirection, float fDistance);
	void Move(DirectX::XMFLOAT3& shift, bool updateVelocity);
	void Move(float x, float y, float z);
	
	void Rotate(float fPitch, float fYaw, float fRoll);
	void SetCameraOffset(const DirectX::XMFLOAT3& cameraOffset);
	void Update(float fElapsedTime = 0.016f);
	virtual void OnUpdateTransform();
	virtual void Animate(float fElapsedTime);

	void SetCamera(CCamera* pCamera) {
		m_pCamera = pCamera;
	}
	CCamera* GetCamera() { return m_pCamera; }
};

class CAirplanePlayer : public CPlayer {
public:
	CAirplanePlayer() {};
	virtual ~CAirplanePlayer() {};

	virtual void OnUpdateTransform();
};