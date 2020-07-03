#pragma once

#define ASPECT_RATIO (float(FRAME_BUFFER_WIDTH) / float(FRAME_BUFFER_HEIGHT))

struct VS_CB_CAMERA_INFO {
	XMFLOAT4X4 viewMatrix{};
	XMFLOAT4X4 projectMatrix{};
};

class CCamera{
protected:
	XMFLOAT4X4 viewMatrix{};
	XMFLOAT4X4 projectMatrix{};

	D3D12_VIEWPORT viewport{};
	D3D12_RECT scissorRect{};

	BoundingFrustum frustum{};
public:
	CCamera();
	virtual ~CCamera();

	void GenerateFrustum();
	bool IsInFrustum(BoundingOrientedBox& boundBox);


	virtual void CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* commandList);
	
	void GenerateViewMatrix(XMFLOAT3 pos, XMFLOAT3 look, XMFLOAT3 up);

	void GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float
		fAspectRatio, float fFOVAngle);
	void SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ =
		0.0f, float fMaxZ = 1.0f);
	void SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom);
	virtual void SetViewportsAndScissorRects(ID3D12GraphicsCommandList* pd3dCommandList);

};

