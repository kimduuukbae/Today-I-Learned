#include "stdafx.h"
#include "Scene.h"
#include "GraphicsPipeline.h"

void CScene::BuildObjects(){
	CCubeMesh* pCubeMesh = new CCubeMesh(8.0f, 8.0f, 8.0f);
	
	m_nObjects = 2;
	m_ppObjects = new CGameObject*[m_nObjects];

	m_ppObjects[0] = new CGameObject();
	m_ppObjects[0]->SetMesh(pCubeMesh);
	m_ppObjects[0]->SetPosition(-8.5f, 0.0f, -14.0f);
	m_ppObjects[0]->SetRotation(0.0f, 0.0f, 0.0f);
	m_ppObjects[0]->SetRotationSpeed(5.0f, 30.0f, 9.0f);
	m_ppObjects[0]->SetColor(RGB(255, 0, 0));

	m_ppObjects[1] = new CGameObject();
	m_ppObjects[1]->SetMesh(pCubeMesh);
	m_ppObjects[1]->SetPosition(8.5f, 0.0f, -14.0f);
	m_ppObjects[1]->SetRotation(0.0f, 0.0f, 0.0f);
	m_ppObjects[1]->SetRotationSpeed(30.0f, 9.0f, 5.0f);
	m_ppObjects[1]->SetColor(RGB(0.0f, 0.0f, 255));
}

void CScene::ReleaseObjects(){
	for (int i = 0; i < m_nObjects; ++i)
		if (m_ppObjects[i])
			delete m_ppObjects[i];
	if (m_ppObjects)
		delete[] m_ppObjects;
}

void CScene::Animate(float fElapsedTime){
	for (int i = 0; i < m_nObjects; ++i)
		m_ppObjects[i]->Animate(fElapsedTime);
}

void CScene::Render(HDC hDCFrameBuffer, CCamera* pCamera){
	if (pCamera)
		CGraphicsPipeline::SetCamera(pCamera);
	for (int i = 0; i < m_nObjects; ++i) {
		CGraphicsPipeline::SetGameObject(m_ppObjects[i]);
		m_ppObjects[i]->Render(hDCFrameBuffer);
	}
}
