#include "stdafx.h"
#include "GraphicsPipeline.h"

using namespace DirectX;

XMFLOAT4X4* CGraphicsPipeline::pWorldMatrix = nullptr;
XMFLOAT4X4* CGraphicsPipeline::pViewProjectMatrix = nullptr;
CViewport* CGraphicsPipeline::pViewport = nullptr;

XMFLOAT3 CGraphicsPipeline::ScreenTransform(const XMFLOAT3& f3Project){
	XMFLOAT3 f3Screen{ f3Project };
	float fHalfWidth{ pViewport->m_nWidth * 0.5f };
	float fHalfHeight{ pViewport->m_nHeight * 0.5f };

	f3Screen.x = pViewport->m_nLeft + (f3Project.x * fHalfWidth) + fHalfWidth;
	f3Screen.y = pViewport->m_nTop + (-f3Project.y * fHalfHeight) + fHalfHeight;

	return f3Screen;
}

XMFLOAT3 CGraphicsPipeline::Project(XMFLOAT3& f3Model) {
	XMMATRIX tModelToProject{ XMMatrixMultiply(XMLoadFloat4x4(pWorldMatrix),
		XMLoadFloat4x4(pViewProjectMatrix)) };
	XMFLOAT3 tProject{};
	XMStoreFloat3(&tProject,
		XMVector3TransformCoord(XMLoadFloat3(&f3Model), tModelToProject));
	return tProject;
}
