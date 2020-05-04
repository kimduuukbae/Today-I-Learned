#include "stdafx.h"
#include "GraphicsPipeline.h"
#include "Camera.h"
#include "GameObject.h"

using namespace DirectX;

CGameObject* CGraphicsPipeline::m_pGameObject = nullptr;
CCamera* CGraphicsPipeline::m_pCamera = nullptr;

XMVECTOR CGraphicsPipeline::ScreenTransform(const XMFLOAT3& f3Projection){
	return m_pCamera->ScreenTransform(XMLoadFloat3(&f3Projection));
}

XMVECTOR CGraphicsPipeline::Project(XMFLOAT3& f3Model) {
	XMVECTOR f3World{ m_pGameObject->WorldTransform(XMLoadFloat3(&f3Model)) };
	XMVECTOR f3Camera{ m_pCamera->CameraTransform(f3World) };
	XMVECTOR f3Projection{ m_pCamera->ProjectionTransform(f3Camera) };
	return f3Projection;
}
