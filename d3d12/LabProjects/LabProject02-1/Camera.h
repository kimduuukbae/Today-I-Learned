#pragma once
#include "Mesh.h"

class CViewport {
public:
	CViewport(int nLeft, int nTop, int nWidth, int nHeight) : 
	m_nLeft { nLeft },
	m_nTop { nTop },
	m_nWidth { nWidth },
	m_nHeight { nHeight } 
	{}

	int m_nLeft;
	int m_nTop;
	int m_nWidth;
	int m_nHeight;
};

class CCamera{
public:
	CCamera() {}
	virtual ~CCamera();

private:
	DirectX::XMFLOAT3 position{};
	DirectX::XMFLOAT3 rotation{};

	// 카메라의 시야각, 투영 사각형가지의 거리
	float m_fFOVAngle { 90.0f };
	float m_fProjectRectDistance { 1.0f };

	CViewport* m_pViewport { nullptr };

	float m_fAspectRatio { float(FRAMEBUFFER_WIDTH) / float(FRAMEBUFFER_HEIGHT) };

public:
	DirectX::XMVECTOR XM_CALLCONV CameraTransform(DirectX::FXMVECTOR& f3World);
	DirectX::XMVECTOR XM_CALLCONV ProjectionTransform(DirectX::FXMVECTOR& f3Camera);
	DirectX::XMVECTOR XM_CALLCONV ScreenTransform(DirectX::FXMVECTOR& f3Projection);

	void SetPosition(float x, float y, float z) {
		position.x = x;
		position.y = y;
		position.z = z;
	}
	void SetRotation(float fPitch, float fYaw, float fRoll) {
		rotation.x = fPitch;
		rotation.y = fYaw;
		rotation.z = fRoll;
	}
	void SetViewport(int nLeft, int nTop, int nWidth, int nHeight);
	void SetFOVAngle(float fFOVAngle);

	void Move(float x, float y, float z);
	void Rotate(float fPitch, float fYaw, float fRoll);
};

