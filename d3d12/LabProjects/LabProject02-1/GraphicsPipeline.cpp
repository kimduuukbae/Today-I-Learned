#include "stdafx.h"
#include "GraphicsPipeline.h"
#include "Camera.h"
#include "GameObject.h"

CGameObject* CGraphicsPipeline::m_pGameObject = nullptr;
CCamera* CGraphicsPipeline::m_pCamera = nullptr;

CPoint3D CGraphicsPipeline::ScreenTransform(CPoint3D& f3Projection){
	CPoint3D f3Screen = m_pCamera->ScreenTransform(f3Projection);
	return f3Screen;
}

CPoint3D CGraphicsPipeline::Project(CPoint3D& f3Model){
	CPoint3D f3World = m_pGameObject->WorldTransfrom(f3Model);
	CPoint3D f3Camera = m_pCamera->CameraTransform(f3World);
	CPoint3D f3Projection = m_pCamera->ProjectionTransform(f3Camera);
	return f3Projection;
}
