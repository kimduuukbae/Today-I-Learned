#include "stdafx.h"
#include "EnemyBox.h"
#include "Mesh.h"

CEnemyBox::CEnemyBox() {
	XMVECTOR minVect{ XMVectorSet(-2.0f, -2.0f, -2.0f, 1.0f) };
	XMVECTOR maxVect{ XMVectorSet(2.0f, 2.0f, 2.0f, 1.0f) };

	BoundingBox::CreateFromPoints(boundBox, minVect, maxVect);
}

CEnemyBox::~CEnemyBox() {

}

void CEnemyBox::Animate(float elapsedTime){
	CRotatingObject::Animate(elapsedTime);
	SetPosition(Vector3::Add(GetPosition(), Vector3::ScalarProduct(direction, elapsedTime)));
	auto [x, y, z] {GetPosition()};
	if (x > 200.0f || x < -200.0f)
		direction.x = -direction.x;
	if (y > 200.0f || y < -200.0f)
		direction.y = -direction.y;
	if (z > 300.0f || z < -300.0f)
		direction.z = -direction.z;
	if (!isLive)
		deletedTime += elapsedTime;
}

bool CEnemyBox::CheckRayIntersection(const XMFLOAT3& rayPos, const XMFLOAT3& rayDir, float* distance){
	XMVECTOR xmrayOrigin{ XMLoadFloat3(&rayPos) };
	XMVECTOR xmrayDir{ XMLoadFloat3(&rayDir) };
	float dist{};
	BoundingBox curColl{ boundBox };
	curColl.Transform(curColl, XMLoadFloat4x4(&worldMatrix));
	return curColl.Intersects(xmrayOrigin, xmrayDir, dist);
}


CEnemyBoxShader::CEnemyBoxShader() {

}

CEnemyBoxShader::~CEnemyBoxShader() {
	for (auto& it : particleObject)
		it->Release();

	for (auto& it : particles)
		it->Release();

	for (auto& it : dieObject)
		it->Release();
}

void CEnemyBoxShader::CreateShaderVariables(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList) {
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL,
		(sizeof(VS_VB_INSTANCE) * maxEnemyBoxCount) + (sizeof(VS_VB_INSTANCE) * 1000) , D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_GENERIC_READ, NULL);
	m_pd3dcbGameObjects->Map(0, NULL, (void**)&m_pcbMappedGameObjects);

	for (int i = 0; i < maxEnemyBoxCount; ++i) {
		CEnemyBox* enemyBox{ new CEnemyBox{} };
		enemyBox->SetPosition(XMFLOAT3(rand()% 100, rand()%100, rand()%100));
		enemyBox->SetRotationSpeed(float(rand() % 100) / 1.1f);
		enemyBox->SetRotatinAxis(XMFLOAT3(rand() % 90, rand() % 90, rand() % 90));
		enemyBox->SetMesh(enemyBoxMesh);
		enemyBox->SetDirection(XMFLOAT3{ RANDOM_F(1), RANDOM_F(1), RANDOM_F(1) });
		m_ppObjects.push_back(enemyBox);
	}


	for (int i = 0; i < 1000; ++i) {
		CEnemyBoxParticle* particle{ new CEnemyBoxParticle{} };
		particle->SetMesh(particleBoxMesh);
		particle->SetDirection(XMFLOAT3{ -RANDOM_F(1.0f) + RANDOM_F(1.0f), -RANDOM_F(1.0f) + RANDOM_F(1.0f
		), -RANDOM_F(1.0f) + RANDOM_F(1.0f) });
		particles.push_back(particle);
	}
}

void CEnemyBoxShader::UpdateShaderVariables(ID3D12GraphicsCommandList
	* pd3dCommandList) {
	pd3dCommandList->SetGraphicsRootShaderResourceView(2,
		m_pd3dcbGameObjects->GetGPUVirtualAddress());

	int j{ 0 };
	for (auto& it : m_ppObjects) {
		m_pcbMappedGameObjects[j].color = XMFLOAT4{ 0.0f, 1.0f, 0.0f, 1.0f };
		XMStoreFloat4x4(&m_pcbMappedGameObjects[j].transform,
			XMMatrixTranspose(XMLoadFloat4x4(&it->worldMatrix)));
		++j;
	}
	for (auto& it : particleObject) {
		m_pcbMappedGameObjects[j].color = XMFLOAT4{ 0.0f, 0.0f, 0.0f, 1.0f };
		XMStoreFloat4x4(&m_pcbMappedGameObjects[j].transform,
			XMMatrixTranspose(XMLoadFloat4x4(&it->worldMatrix)));
		++j;
	}
}

void CEnemyBoxShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
	* pd3dCommandList) {
	enemyBoxMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList,
		4.0f, 4.0f, 4.0f);
	particleBoxMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList,
		2.0f, 2.0f, 2.0f, XMFLOAT4{ 1.0f, 1.0f, 1.0f, 1.0f });
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}


void CEnemyBoxShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera
	* pCamera) {
	CObjectsShader::Render(pd3dCommandList, pCamera);
	UpdateShaderVariables(pd3dCommandList);
	if (m_ppObjects.size())
		m_ppObjects.front()->Render(pd3dCommandList, pCamera, m_ppObjects.size() + particleObject.size());
}

void CEnemyBoxShader::AnimateObjects(float elapsedTime) {
	auto end{ m_ppObjects.end() };
	for (auto it = m_ppObjects.begin(); it != end;) {
		CEnemyBox* box{ reinterpret_cast<CEnemyBox*>((*it)) };
		box->Animate(elapsedTime);
		if (!box->GetLive()) {
			dieObject.push_back(box);
			for (int i = 0; i < 100; ++i) {
				particles.back()->SetPosition(box->GetPosition());
				particles.back()->SetParent(box);
				particleObject.push_back(particles.back());
				particles.pop_back();
			}
			it = m_ppObjects.erase(it);
		}
		else
			++it;
	}
	for (auto& it : particleObject) 
		it->Animate(elapsedTime);

	auto end2{ dieObject.end() };
	for (auto it = dieObject.begin(); it != end2; ) {
		(*it)->Animate(elapsedTime);
		if ((*it)->GetDeletedTime() > 3.0f) {
			particleObject.erase(std::remove_if(particleObject.begin(), particleObject.end(), [&](CEnemyBoxParticle* p) {
				return p->GetParent() == (*it);
				}), particleObject.end());
			m_ppObjects.push_back(*it);
			(*it)->SetLive(true);
			it = dieObject.erase(it);
		}
		else
			++it;
	}
}

void CEnemyBoxShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature* pd3dGraphicsRootSignature){
	pipelineStateCount = 1;
	pipelineStates = new ID3D12PipelineState * [pipelineStateCount];
	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature, false);
}

CGameObject* CEnemyBoxShader::IsPickingObject(const XMFLOAT3& rayDirOrigin, const XMFLOAT3& rayDir){
	for (auto& it : m_ppObjects) {
		CEnemyBox* box{ reinterpret_cast<CEnemyBox*>(it) };
		if (box->CheckRayIntersection(rayDirOrigin, rayDir, nullptr)) 
			return box;
	}
	return nullptr;
}

CEnemyBoxParticle::CEnemyBoxParticle(){

}

CEnemyBoxParticle::~CEnemyBoxParticle(){

}

void CEnemyBoxParticle::Animate(float elapsedTime){
	CGameObject::Animate(elapsedTime);
	SetPosition(Vector3::Add(GetPosition(), Vector3::ScalarProduct(direction, elapsedTime)));
}
