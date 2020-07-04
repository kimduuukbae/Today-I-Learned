#pragma once
#include "Mesh.h"

class CShader;
class CCamera;

class CGameObject{
public:
	CGameObject();
	virtual ~CGameObject();
	void AddRef() { ++ref; }
	void Release() { mesh = nullptr;  if (--ref <= 0) delete this; }
	void ReleaseUploadBuffers();
	
	virtual void SetMesh(CMesh* inMesh);
	virtual void SetShader(CShader* inShader);
	virtual void Animate(float elapsedTime);

	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList* commandList, CCamera* pCamera);
	virtual void Render(ID3D12GraphicsCommandList* commandList, CCamera* pCamera, UINT nInstances);

	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);
	void Rotate(float fPitch, float fYaw, float fRoll);

	virtual void CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* commandList);
	virtual void ReleaseShaderVariables();

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	void SetPosition(float x, float y, float z);
	void SetPosition(const XMFLOAT3& pos);

	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	XMFLOAT4X4 worldMatrix{};
protected:
	
	CMesh* mesh{ nullptr };
	CShader* shader{ nullptr };
private:
	int ref{};
};

class CRotatingObject : public CGameObject {
public:
	CRotatingObject();
	virtual ~CRotatingObject();

	void SetRotationSpeed(float fRotationSpeed) { rotationSpeed = fRotationSpeed; }
	void SetRotatinAxis(const XMFLOAT3& axis) { rotatingAxis = axis; }

	virtual void Animate(float elapsedTime) override;
private:
	XMFLOAT3 rotatingAxis{};
	float rotationSpeed{};
};

