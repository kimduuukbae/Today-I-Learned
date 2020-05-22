#pragma once
#include "Mesh.h"

class CPlayer;

class CViewport {
public:
	CViewport() {}
	CViewport(int nLeft, int nTop, int nWidth, int nHeight) : 
	m_nLeft { nLeft },
	m_nTop { nTop },
	m_nWidth { nWidth },
	m_nHeight { nHeight } 
	{}

	void SetViewport(int nLeft, int nTop, int nWidth, int nHeight);
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
	//월드좌표계 카메라의 위치
	DirectX::XMFLOAT3 position{ 0.0f,0.0f,0.0f };
	DirectX::XMFLOAT3 right{ 1.0f,0.0f,0.0f };
	DirectX::XMFLOAT3 up{ 0.0f, 1.0f, 0.0f };
	DirectX::XMFLOAT3 look{ 0.0f, 0.0f,1.0f };

	//카메라 변환 행렬
	DirectX::XMFLOAT4X4 viewMatrix{ Matrix4x4::identity() };
	// 원근 투영 변환 행렬
	DirectX::XMFLOAT4X4 projectMatrix{ Matrix4x4::identity() };
	

	// 카메라의 시야각, 투영 사각형가지의 거리
	float m_fFOVAngle { 90.0f };
	float m_fProjectRectDistance { 1.0f };

	float m_fAspectRatio { float(FRAMEBUFFER_WIDTH) / float(FRAMEBUFFER_HEIGHT) };

public:
	// 카메라 * 원근 투영 변환 행렬
	DirectX::XMFLOAT4X4 viewProjectMatrix{ Matrix4x4::identity() };
	CViewport m_Viewport{};
	void GenerateViewMatrix();
	void GeneratePerspectiveProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fFovAngle);
	void SetViewport(int nLeft, int nTop, int nWidth, int nHeight);
	void SetFOVAngle(float fFOVAngle);
	
	//카메라가 바라보는 지점
	void SetLookAt(DirectX::XMFLOAT3& lookAt, DirectX::XMFLOAT3& up);
	void SetLookAt(DirectX::XMFLOAT3& pos, DirectX::XMFLOAT3& lookAt, DirectX::XMFLOAT3& up);
	
	void Move(float x, float y, float z);
	void Move(DirectX::XMFLOAT3& shift);

	void Rotate(float fPitch, float fYaw, float fRoll);

	void Update(CPlayer* pPlayer, DirectX::XMFLOAT3& lookAt, float fElapsedTime = 0.016f);

	//DirectX::XMVECTOR XM_CALLCONV CameraTransform(DirectX::FXMVECTOR& f3World);
	//DirectX::XMVECTOR XM_CALLCONV ProjectionTransform(DirectX::FXMVECTOR& f3Camera);
	//DirectX::XMVECTOR XM_CALLCONV ScreenTransform(DirectX::FXMVECTOR& f3Projection);

	void SetPosition(float x, float y, float z);
	void SetRotation(float fPitch, float fYaw, float fRoll);


	
	
};

