#pragma once

#define FIRST_PERSON_CAMERA 1
#define SPACESHIP_CAMERA	2
#define THIRD_PERSON_CAMERA	3


#define ASPECT_RATIO (float(FRAME_BUFFER_WIDTH) / float(FRAME_BUFFER_HEIGHT))

class CPlayer;

struct VS_CB_CAMERA_INFO {
	XMFLOAT4X4 viewMatrix{};
	XMFLOAT4X4 projectMatrix{};
};

class CCamera{
protected:
	XMFLOAT3 position{};
	XMFLOAT3 right{};
	XMFLOAT3 up{};
	XMFLOAT3 look{};

	float pitch{};
	float roll{};
	float yaw{};

	DWORD mode{};
	XMFLOAT3 lookAtWorld{};
	XMFLOAT3 offset{};
	float timeLag{};

	XMFLOAT4X4 viewMatrix{};
	XMFLOAT4X4 projectMatrix{};

	D3D12_VIEWPORT viewport{};
	D3D12_RECT scissorRect{};

	CPlayer* m_pPlayer{ nullptr };
public:
	CCamera();
	CCamera(CCamera* pCamera);
	virtual ~CCamera();

	virtual void CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* commandList);
	
	void GenerateViewMatrix();
	void GenerateViewMatrix(XMFLOAT3 pos, XMFLOAT3 lookAt, XMFLOAT3 fup);
	void RegenerateViewMatrix();

	void GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float
		fAspectRatio, float fFOVAngle);
	void SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ =
		0.0f, float fMaxZ = 1.0f);
	void SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom);
	virtual void SetViewportsAndScissorRects(ID3D12GraphicsCommandList* pd3dCommandList);

	void SetPlayer(CPlayer* pPlayer) { m_pPlayer = pPlayer; }
	CPlayer* GetPlayer() { return m_pPlayer; }

	void SetMode(DWORD nMode) { mode = nMode; }
	DWORD GetMode() { return mode; }

	void SetPosition(XMFLOAT3 f3Position) { position = f3Position; }
	XMFLOAT3& GetPosition() { return position; }

	void SetLookAtPosition(const XMFLOAT3& f3LookAtWorld) { lookAtWorld = f3LookAtWorld; }
	XMFLOAT3& GetLookAtPosition() { return lookAtWorld; }

	XMFLOAT3& GetRightVector() { return right; }
	XMFLOAT3& GetUpVector() { return up; }
	XMFLOAT3& GetLookVector() { return look; }

	float GetPitch() { return pitch; }
	float GetRoll() { return roll; }
	float GetYaw() { return yaw; }

	void SetOffset(const XMFLOAT3& f3Offset) { offset = f3Offset; }
	XMFLOAT3& GetOffset() { return offset; }

	void SetTimeLag(float fTimeLag) { timeLag = fTimeLag; }
	float GetTimeLag() { return timeLag; }

	XMFLOAT4X4 GetViewMatrix() { return viewMatrix; }
	XMFLOAT4X4 GetProjectionMatrix() { return projectMatrix; }
	D3D12_VIEWPORT GetViewport() { return viewport; }
	D3D12_RECT GetScissorRecT() { return scissorRect; }

	virtual void Move(const XMFLOAT3& shift);
	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f) {}
	virtual void Update(const XMFLOAT3& f3LookAt, float felapsedTime) {}
	virtual void SetLookAt(XMFLOAT3& f3LookAt) {}
};

class CSpaceShipCamera : public CCamera{
public:
	CSpaceShipCamera(CCamera* pCamera);
	virtual ~CSpaceShipCamera() { }
	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);
};

class CFirstPersonCamera : public CCamera{
public:
	CFirstPersonCamera(CCamera* pCamera);
	virtual ~CFirstPersonCamera() { }
	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);
};

class CThirdPersonCamera : public CCamera {
public:
	CThirdPersonCamera(CCamera* pCamera);
	virtual ~CThirdPersonCamera() { }
	virtual void Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed);
	virtual void SetLookAt(XMFLOAT3& vLookAt);
};