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

	static DirectX::XMVECTOR ScreenTransform(const DirectX::XMFLOAT3& f3Projection);
	static DirectX::XMVECTOR Project(DirectX::XMFLOAT3& f3Model);
};

