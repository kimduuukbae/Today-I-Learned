#pragma once
#include "GameObject.h"
#include "Camera.h"

class CGraphicsPipeline{
private:
	// 게임객체의 월드 변환 행렬
	static DirectX::XMFLOAT4X4* pWorldMatrix;
	static DirectX::XMFLOAT4X4* pViewProjectMatrix;
	static CViewport* pViewport;

public:
	static void SetWorldTransform(DirectX::XMFLOAT4X4* world) {
		pWorldMatrix = world;
	}

	static void SetViewProjectTransform(DirectX::XMFLOAT4X4* viewProject) {
		pViewProjectMatrix = viewProject;
	}

	static void SetViewport(CViewport* pTViewport) {
		pViewport = pTViewport;
	}

	static DirectX::XMFLOAT3 ScreenTransform(const DirectX::XMFLOAT3& f3Project);
	static DirectX::XMFLOAT3 Project(DirectX::XMFLOAT3& f3Model);
};

