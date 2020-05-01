#include "stdafx.h"
#include "Mesh.h"
#include "GraphicsPipeline.h"
using namespace DirectX;

CPolygon::CPolygon(int nVertices){
	m_nVertices = nVertices;
	m_pVertices = new CVertex[nVertices];
}

CPolygon::~CPolygon() {
	if (m_pVertices) delete[] m_pVertices;
}

void CPolygon::SetVertex(int nIndex, CVertex vertex) {
	if ((0 <= nIndex) && (nIndex < m_nVertices) && m_pVertices)
		m_pVertices[nIndex] = vertex;
}

CMesh::CMesh(int nPolygons) {
	m_nPolygons = nPolygons;
	m_ppPolygons = new CPolygon*[nPolygons];
}

CMesh::~CMesh() {
	if (m_ppPolygons) {
		for (int i = 0; i < m_nPolygons; ++i)
			if (m_ppPolygons[i])
				delete m_ppPolygons[i];
		delete[] m_ppPolygons;
	}
}

void CMesh::SetPolygon(int nIndex, CPolygon* pPolygon) {
	if ((0 <= nIndex) && (nIndex < m_nPolygons))
		m_ppPolygons[nIndex] = pPolygon;
}

void Draw2DLine(HDC hDCFrameBuffer, XMFLOAT3& f3PreviousProject, XMFLOAT3& f3CurrentPorject) {
	XMFLOAT3 f3Previous{}, f3Current{};
	XMStoreFloat3(&f3Previous, CGraphicsPipeline::ScreenTransform(f3PreviousProject));
	XMStoreFloat3(&f3Current, CGraphicsPipeline::ScreenTransform(f3CurrentPorject));
	::MoveToEx(hDCFrameBuffer, (long)f3Previous.x, (long)f3Previous.y, nullptr);
	::LineTo(hDCFrameBuffer, (long)f3Current.x, (long)f3Current.y);
}

void CMesh::Render(HDC hDCFrameBuffer) {
	XMFLOAT3 f3InitialProject, f3PreviousProject;
	bool bPreviousInside = false, bInitialInside = false,
		bCurrentInside = false, bIntersectInside = false;

	// 메쉬 다각형 렌더링
	for (int j = 0; j < m_nPolygons; ++j) {
		int nVertices = m_ppPolygons[j]->m_nVertices;
		CVertex* pVertices = m_ppPolygons[j]->m_pVertices;

		XMStoreFloat3(&f3InitialProject, CGraphicsPipeline::Project(pVertices[0].position));
		f3PreviousProject = f3InitialProject;

		// 변환된 점이 투영 사각형에 포함되는지 확인
		bPreviousInside = bInitialInside = (-1.0f <= f3InitialProject.x) && (f3InitialProject.x <= 1.0f)
			&& (-1.0f <= f3InitialProject.y) && (f3InitialProject.y <= 1.0f);
		for (int i = 1; i < nVertices; ++i) {
			XMFLOAT3 f3CurrentProject{};
			XMStoreFloat3(&f3CurrentProject, CGraphicsPipeline::Project(pVertices[i].position));
			bCurrentInside = (-1.0f <= f3CurrentProject.x) && (f3CurrentProject.x <= 1.0f)
				&& (-1.0f <= f3CurrentProject.y) && (f3CurrentProject.y <= 1.0f);
			if (((f3PreviousProject.z >= 0.0f) || (f3CurrentProject.z >= 0.0f))
				&& ((bCurrentInside || bPreviousInside))) 
				::Draw2DLine(hDCFrameBuffer, f3PreviousProject, f3CurrentProject);
			
			f3PreviousProject = f3CurrentProject;
			bPreviousInside = bCurrentInside;
		}
		if (((f3PreviousProject.z >= 0.0f) || (f3InitialProject.z >= 0.0f))
			&& ((bInitialInside || bPreviousInside))) 
			::Draw2DLine(hDCFrameBuffer, f3PreviousProject, f3InitialProject);
		
	}
}

CCubeMesh::CCubeMesh(float fWidth, float fHeight, float fDepth) : CMesh(6){
	float fHalfWidth = { fWidth * 0.5f };
	float fHalfHeight = { fHeight * 0.5f };
	float fHalfDepth = { fDepth * 0.5f };

	CPolygon* pFrontFace = new CPolygon(4);
	pFrontFace->SetVertex(0, CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(1, CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	pFrontFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	SetPolygon(0, pFrontFace);

	CPolygon* pTopFace = new CPolygon(4);
	pTopFace->SetVertex(0, CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	pTopFace->SetVertex(1, CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pTopFace->SetVertex(2, CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pTopFace->SetVertex(3, CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));

	SetPolygon(1, pTopFace);

	CPolygon* pBackFace = new CPolygon(4);
	pBackFace->SetVertex(0, CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(1, CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(2, CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pBackFace->SetVertex(3, CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	SetPolygon(2, pBackFace);

	CPolygon* pBottomFace = new CPolygon(4);
	pBottomFace->SetVertex(0, CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	pBottomFace->SetVertex(1, CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	pBottomFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pBottomFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	SetPolygon(3, pBottomFace);

	CPolygon* pLeftFace = new CPolygon(4);
	pLeftFace->SetVertex(0, CVertex(-fHalfWidth, +fHalfHeight, +fHalfDepth));
	pLeftFace->SetVertex(1, CVertex(-fHalfWidth, +fHalfHeight, -fHalfDepth));
	pLeftFace->SetVertex(2, CVertex(-fHalfWidth, -fHalfHeight, -fHalfDepth));
	pLeftFace->SetVertex(3, CVertex(-fHalfWidth, -fHalfHeight, +fHalfDepth));
	SetPolygon(4, pLeftFace);

	CPolygon* pRightFace = new CPolygon(4);
	pRightFace->SetVertex(0, CVertex(+fHalfWidth, +fHalfHeight, -fHalfDepth));
	pRightFace->SetVertex(1, CVertex(+fHalfWidth, +fHalfHeight, +fHalfDepth));
	pRightFace->SetVertex(2, CVertex(+fHalfWidth, -fHalfHeight, +fHalfDepth));
	pRightFace->SetVertex(3, CVertex(+fHalfWidth, -fHalfHeight, -fHalfDepth));
	SetPolygon(5, pRightFace);
}
