#pragma once
#include "Mesh.h"
class CGameObject;
class CCamera;

class CGraphicsPipeline{
private:
	static CGameObject* m_pGameObject;
	static CCamera* m_pCamera;
public:
	static void SetGameObject(CGameObject* pGameObject) {
		m_pGameObject = pGameObject;
	}
	static void SetCamera(CCamera* pCamera) {
		m_pCamera = pCamera;
	}

	static CPoint3D ScreenTransform(CPoint3D& f3Projection);
	static CPoint3D Project(CPoint3D& f3Model);
};

