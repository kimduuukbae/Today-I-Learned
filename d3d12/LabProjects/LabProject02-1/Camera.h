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
	float m_fxPosition = { 0.0f };
	float m_fyPosition = { 0.0f };
	float m_fzPosition = { 0.0f };

	float m_fxRotation = { 0.0f };
	float m_fyRotation = { 0.0f };
	float m_fzRotation = { 0.0f };

	// 카메라의 시야각, 투영 사각형가지의 거리
	float m_fFOVAngle = { 90.0f };
	float m_fProjectRectDistance = { 1.0f };

	CViewport* m_pViewport = { nullptr };

	float m_fAspectRatio = { float(FRAMEBUFFER_WIDTH) / float(FRAMEBUFFER_HEIGHT) };

public:
	CPoint3D CameraTransform(CPoint3D& f3World);
	CPoint3D ProjectionTransform(CPoint3D& f3Camera);
	CPoint3D ScreenTransform(CPoint3D& f3Projection);

	void SetPosition(float x, float y, float z) {
		m_fxPosition = x;
		m_fyPosition = y;
		m_fzPosition = z;
	}
	void SetRotation(float fPitch, float fYaw, float fRoll) {
		m_fxRotation = fPitch;
		m_fyRotation = fYaw;
		m_fzRotation = fRoll;
	}
	void SetViewport(int nLeft, int nTop, int nWidth, int nHeight);
	void SetFOVAngle(float fFOVAngle);

	void Move(float x, float y, float z);
	void Rotate(float fPitch, float fYaw, float fRoll);
};

