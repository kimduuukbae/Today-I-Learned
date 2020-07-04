#pragma once
#include "Shader.h"
#include "GameObject.h"


class CEnemyBox : public  CRotatingObject{
public:
	CEnemyBox();
	~CEnemyBox();
	void SetDirection(const XMFLOAT3& dir) {
		direction = Vector3::Normalize(dir);
	}
	virtual void Animate(float elapsedTime) override;

	bool CheckRayIntersection(const XMFLOAT3& rayPos, const XMFLOAT3& rayDir, float* distance);
private:
	XMFLOAT3 direction{};
	BoundingBox boundBox{};
};

class CEnemyBoxShader : public CInstancingShader{
public:
	CEnemyBoxShader();
	virtual ~CEnemyBoxShader();
	virtual void CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList);
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList);
	virtual void BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
		* pd3dCommandList);
	virtual void Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera);

	virtual void AnimateObjects(float elapsedTime) override;

	CGameObject* IsPickingObject(const XMFLOAT3& rayDirOrigin, const XMFLOAT3& rayDir);
protected:

	ID3D12Resource* m_pd3dcbGameObjects = NULL;
	VS_VB_INSTANCE* m_pcbMappedGameObjects = NULL;
	CCubeMeshDiffused* enemyBoxMesh{ nullptr };
	const int maxEnemyBoxCount{ 10 };

};

