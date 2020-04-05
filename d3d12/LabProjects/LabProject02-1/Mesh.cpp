#include "stdafx.h"
#include "Mesh.h"
#include "GraphicsPipeline.h"


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

void Draw2DLine(HDC hDCFrameBuffer, CPoint3D& f3PreviousProject, CPoint3D& f3CurrentPorject) {
	CPoint3D f3Previous = CGraphicsPipeline::ScreenTransform(f3PreviousProject);
	CPoint3D f3Current = CGraphicsPipeline::ScreenTransform(f3CurrentPorject);
	::MoveToEx(hDCFrameBuffer, (long)f3Previous.x, (long)f3Previous.y, nullptr);
	::LineTo(hDCFrameBuffer, (long)f3Current.x, (long)f3Current.y);
}

void CMesh::Render(HDC hDCFrameBuffer) {
	CPoint3D f3InitialProject, f3PreviousProject, f3Intersect;
	bool bPreviousInside = false, bInitialInside = false,
		bCurrentInside = false, bIntersectInside = false;

	// 메쉬 다각형 렌더링
	for (int j = 0; j < m_nPolygons; ++j) {
		int nVertices = m_ppPolygons[j]->m_nVertices;
		CVertex* pVertices = m_ppPolygons[j]->m_pVertices;

		f3PreviousProject = f3InitialProject = CGraphicsPipeline::Project(pVertices[0].m_f3Position);
		// 변환된 점이 투영 사각형에 포함되는지 확인
		bPreviousInside = bInitialInside = (-1.0f <= f3InitialProject.x) && (f3InitialProject.x <= 1.0f)
			&& (-1.0f <= f3InitialProject.y) && (f3InitialProject.y <= 1.0f);
		for (int i = 1; i < nVertices; ++i) {
			CPoint3D f3CurrentProject = CGraphicsPipeline::Project(pVertices[i].m_f3Position);
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

CCubeMesh::CCubeMesh(float fWidth, float fHeight, float fDepth){

}
