#pragma once
#include "Mesh.h"

class CShader;

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
	virtual void Render(ID3D12GraphicsCommandList* commandList);
protected:
	XMFLOAT4X4 worldMatrix{};
	CMesh* mesh{ nullptr };
	CShader* shader{ nullptr };
private:
	int ref{};
};

