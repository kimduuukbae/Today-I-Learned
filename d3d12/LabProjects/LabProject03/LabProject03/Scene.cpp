#include "stdafx.h"
#include "Scene.h"
#include "GraphicsPipeline.h"

using namespace DirectX;

void CScene::BuildObjects(){
	CCubeMesh* pCubeMesh = new CCubeMesh(4.0f, 4.0f, 4.0f);
	CCubeMesh* pParticleMesh = new CCubeMesh(1.0f, 1.0f, 1.0f);
	m_nObjects = 5;
	m_ppObjects = new CEnemyBox*[m_nObjects];

	m_ppObjects[0] = new CEnemyBox();
	m_ppObjects[0]->SetMesh(pCubeMesh);
	m_ppObjects[0]->SetColor(RGB(255, 0, 0));
	m_ppObjects[0]->SetPosition(-13.5f, 0.0f, +14.0f);
	m_ppObjects[0]->SetRotationAxis(XMFLOAT3{ 1.0f, 1.0f, 0.0f });
	m_ppObjects[0]->SetRotationSpeed(90.0f);
	m_ppObjects[0]->SetMovingDirection(XMFLOAT3(1.0f, 0.0f, 0.0f));
	m_ppObjects[0]->SetMovingSpeed(1.5f);
	m_ppObjects[0]->SetParticles(pParticleMesh, RGB(255, 0, 0));

	m_ppObjects[1] = new CEnemyBox();
	m_ppObjects[1]->SetMesh(pCubeMesh);
	m_ppObjects[1]->SetColor(RGB(0, 0, 255));
	m_ppObjects[1]->SetPosition(+13.5f, 0.0f, +14.0f);
	m_ppObjects[1]->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 1.0f));
	m_ppObjects[1]->SetRotationSpeed(180.0f);
	m_ppObjects[1]->SetMovingDirection(XMFLOAT3(-1.0f, 0.0f, 0.0f));
	m_ppObjects[1]->SetMovingSpeed(3.0f);
	m_ppObjects[1]->SetParticles(pParticleMesh, RGB(0, 0, 255));

	m_ppObjects[2] = new CEnemyBox();
	m_ppObjects[2]->SetMesh(pCubeMesh);
	m_ppObjects[2]->SetColor(RGB(0, 255, 0));
	m_ppObjects[2]->SetPosition(0.0f, +5.0f, 20.0f);
	m_ppObjects[2]->SetRotationAxis(XMFLOAT3(1.0f, 0.0f, 1.0f));
	m_ppObjects[2]->SetRotationSpeed(30.15f);
	m_ppObjects[2]->SetMovingDirection(XMFLOAT3(1.0f, -1.0f, 0.0f));
	m_ppObjects[2]->SetMovingSpeed(5.0f);
	m_ppObjects[2]->SetParticles(pParticleMesh, RGB(0, 255, 0));

	m_ppObjects[3] = new CEnemyBox();
	m_ppObjects[3]->SetMesh(pCubeMesh);
	m_ppObjects[3]->SetColor(RGB(0, 255, 255));
	m_ppObjects[3]->SetPosition(0.0f, 0.0f, 40.0f);
	m_ppObjects[3]->SetRotationAxis(XMFLOAT3(0.0f, 0.0f, 1.0f));
	m_ppObjects[3]->SetRotationSpeed(40.6f);
	m_ppObjects[3]->SetMovingDirection(XMFLOAT3(0.0f, 0.0f, 1.0f));
	m_ppObjects[3]->SetMovingSpeed(2.5f);
	m_ppObjects[3]->SetParticles(pParticleMesh, RGB(0, 255,255));

	m_ppObjects[4] = new CEnemyBox();
	m_ppObjects[4]->SetMesh(pCubeMesh);
	m_ppObjects[4]->SetColor(RGB(128, 0, 255));
	m_ppObjects[4]->SetPosition(10.0f, 10.0f, 50.0f);
	m_ppObjects[4]->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 1.0f));
	m_ppObjects[4]->SetRotationSpeed(50.06f);
	m_ppObjects[4]->SetMovingDirection(XMFLOAT3(0.0f, 1.0f, 1.0f));
	m_ppObjects[4]->SetMovingSpeed(7.0f);
	m_ppObjects[4]->SetParticles(pParticleMesh, RGB(128, 0, 255));

	CMapMesh* pMapMesh = new CMapMesh{ 100.0f, 100.0f, 10.0f };
	m_pMapObjects = new CGameObject[m_nMapObjects];

	for (int i = 0; i < m_nMapObjects; ++i) {
		m_pMapObjects[i].SetMesh(pMapMesh);
		m_pMapObjects[i].SetColor(RGB(0, 0, 0));
		m_pMapObjects[i].SetPosition(0.0f, 0.0f,
			static_cast<float>(i * 10) + m_pPlayer->position.z - m_nMapObjects * 5);
	}
}

void CScene::ReleaseObjects(){
	for (int i = 0; i < m_nObjects; ++i) 
		if (m_ppObjects[i])
			delete m_ppObjects[i];
	
	if (m_ppObjects)
		delete[] m_ppObjects;
	if (m_pMapObjects)
		delete[] m_pMapObjects;
}

void CScene::Animate(float fElapsedTime){
	for (int i = 0; i < m_nObjects; ++i)
		m_ppObjects[i]->Animate(fElapsedTime);

	if (abs(m_pMapObjects[m_nMapObjects / 2].worldMatrix._43 - m_pPlayer->position.z) > 10.0f)
		for (int i = 0; i < m_nMapObjects; ++i)
			m_pMapObjects[i].SetPosition(0.0f, 0.0f,
				static_cast<float>(i * 10) + m_pPlayer->position.z - m_nMapObjects * 5);
}

void CScene::Render(HDC hDCFrameBuffer, CCamera* pCamera){
	CGraphicsPipeline::SetViewport(&pCamera->m_Viewport);
	CGraphicsPipeline::SetViewProjectTransform(&pCamera->viewProjectMatrix);

	for (int i = 0; i < m_nObjects; ++i)
		m_ppObjects[i]->Render(hDCFrameBuffer, pCamera);

	for (int i = 0; i < m_nMapObjects; ++i)
		m_pMapObjects[i].Render(hDCFrameBuffer, pCamera);
}

int CScene::checkObjects(XMFLOAT3& rayOrigin, XMFLOAT3& rayDir){
	// rayOrigin : 광선의 출발점
	// rayDir : 광선의 방향벡터
	int lastCount{ -1 };
	for (int i = 0; i < m_nObjects; ++i) {
		float dist{};
		if (m_ppObjects[i]->GetLive() && m_ppObjects[i]->getCollisionBox().
			Intersects(XMLoadFloat3(&rayOrigin), XMVector4Normalize(XMLoadFloat3(&rayDir)), dist)) {
			if (lastCount != -1) {
				// 오브젝트가 두 개 이상 클릭 됐다면, 둘 중 거리 비교해서 더 가까운 오브젝트 클릭
				XMFLOAT3 last{ m_ppObjects[lastCount]->GetPosition() };
				XMFLOAT3 cur{ m_ppObjects[i]->GetPosition() };
				lastCount = last.z - rayOrigin.z > cur.z - rayOrigin.z ? i : lastCount;
				continue;
			}
			lastCount = i;
		}
	}
	return lastCount;
}

CGameObject* CScene::GetGameObject(int index){
	return m_ppObjects[index];
}

void CScene::ProcessCollision(){
	if (CAirplanePlayer* p = dynamic_cast<CAirplanePlayer*>(m_pPlayer); p) {
		for (auto& bullet : p->GetBullets()) {
			for (int i = 0; i < m_nObjects; ++i) {
				if (m_ppObjects[i]->GetLive() && bullet->getCollisionBox().Contains
				(m_ppObjects[i]->getCollisionBox()) != ContainmentType::DISJOINT) {
					bullet->SetLive(false);
					m_ppObjects[i]->SetLive(false);
				}
			}
		}
	}

	for (int i = 0; i < m_nObjects; ++i) {
		for (int j = 0; j < m_nObjects; ++j) {
			if (m_ppObjects[i] == m_ppObjects[j])
				continue;

			if (m_ppObjects[i]->getCollisionBox().Contains(
				m_ppObjects[j]->getCollisionBox()) != ContainmentType::DISJOINT) {

				XMFLOAT3 tarLhs{ m_ppObjects[i]->GetDirection() };
				tarLhs = XMFLOAT3{ -tarLhs.x, -tarLhs.y, -tarLhs.z };
				XMFLOAT3 tarRhs{ m_ppObjects[j]->GetDirection() };
				tarRhs = XMFLOAT3{ -tarRhs.x, -tarRhs.y, -tarRhs.z };
				m_ppObjects[i]->SetMovingDirection(tarLhs);
				m_ppObjects[j]->SetMovingDirection(tarRhs);
			}
		}
	}

	constexpr float mapCollisionSize{ 100.0f };
	for (int i = 0; i < m_nObjects; ++i) {
		
		XMFLOAT3 pos{ m_ppObjects[i]->GetPosition() };
		XMFLOAT3 dir{ m_ppObjects[i]->GetDirection() };
		if (abs(pos.x > mapCollisionSize))
			dir.x = -dir.x;
		if (abs(pos.y > mapCollisionSize))
			dir.y = -dir.y;
		if (abs(pos.z > mapCollisionSize))
			dir.z = -dir.z;

		m_ppObjects[i]->SetMovingDirection(dir);
	}
}
