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
	bool isCollision(const BoundingBox& target) {
		BoundingBox curColl{ boundBox };
		curColl.Transform(curColl, XMLoadFloat4x4(&worldMatrix));
		return curColl.Contains(target) != ContainmentType::DISJOINT;
	}
	void SetLive(bool bFlag) {
		isLive = bFlag;
		deletedTime = 0.0f;
	}
	bool GetLive() {
		return isLive;
	}
	float GetDeletedTime() {
		return deletedTime;
	}
private:
	XMFLOAT3 direction{};
	BoundingBox boundBox{};
	bool isLive{ true };
	float deletedTime{ 0.0f };
};

class CEnemyBoxParticle : public CGameObject {
public:
	CEnemyBoxParticle();
	~CEnemyBoxParticle();

	virtual void Animate(float elapsedTime) override;

	void SetDirection(const XMFLOAT3& dir) {
		direction = Vector3::Normalize(dir);
	}
	
	void SetParent(CEnemyBox* target) {
		parent = target;
	}
	CEnemyBox* GetParent() {
		return parent;
	}
private:
	XMFLOAT3 direction{};
	CEnemyBox* parent{ nullptr };
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
	virtual void CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature) override;
	CGameObject* IsPickingObject(const XMFLOAT3& rayDirOrigin, const XMFLOAT3& rayDir);

	std::list<CGameObject*>& GetGameObject() {
		return m_ppObjects;
	}
protected:

	ID3D12Resource* m_pd3dcbGameObjects = NULL;
	VS_VB_INSTANCE* m_pcbMappedGameObjects = NULL;

	std::vector<CEnemyBoxParticle*> particles{};
	std::vector<CEnemyBoxParticle*> particleObject{};
	std::list<CEnemyBox*> dieObject{};

	CCubeMeshDiffused* enemyBoxMesh{ nullptr };
	CCubeMeshDiffused* particleBoxMesh{ nullptr };
	const int maxEnemyBoxCount{ 10 };

};

