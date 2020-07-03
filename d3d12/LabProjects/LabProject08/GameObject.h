#pragma once
#include "Mesh.h"

class CShader;
class CCamera;

class CGameObject{
public:
	CGameObject();
	virtual ~CGameObject();
	void AddRef() { ++ref; }
	void Release( ) { if (--ref <= 0) delete this; }
	void ReleaseUploadBuffers();
	
	virtual void SetMesh(CMesh* inMesh);
	virtual void SetShader(CShader* inShader);
	virtual void Animate(float elapsedTime);

	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList* commandList, CCamera* pCamera);

	void Rotate(XMFLOAT3* pxmf3Axis, float fAngle);
protected:
	XMFLOAT4X4 worldMatrix{};
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

