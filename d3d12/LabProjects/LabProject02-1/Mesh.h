#pragma once

class CPoint3D {
public:
	CPoint3D() {}
	CPoint3D(float x, float y, float z) : x{ x }, y{ y }, z{ z } {}
	virtual ~CPoint3D() {}

	float x = { 0.0f };
	float y = { 0.0f };
	float z = { 0.0f };
};

class CVertex {
public:
	CVertex() {}
	CVertex(float x, float y, float z) : m_f3Position{ CPoint3D{x, y, z} } {}
	virtual ~CVertex() { }
	CPoint3D m_f3Position;
};

class CPolygon {
public:
	CPolygon() {}
	CPolygon(int nVertices);
	virtual ~CPolygon();

	int m_nVertices = { 0 };
	CVertex* m_pVertices = { nullptr };
	
	void SetVertex(int nIndex, CVertex vertex);
};

class CMesh{
public:
	CMesh() {}
	CMesh(int nPolygons);
	virtual ~CMesh();

private:
	int m_nReferences = { 0 };	// 메쉬가 공유되는 게임 객체의 개수
public:
	void AddRef() { m_nReferences++; }
	void Release() { 
		--m_nReferences;
		if (m_nReferences <= 0) delete this;
	}

private:
	int m_nPolygons = { 0 };
	CPolygon** m_ppPolygons = { nullptr }; // 메쉬를 구성하는 다각형 (면) 들의 리스트
public:
	void SetPolygon(int nIndex, CPolygon* pPolygon);

	virtual void Render(HDC hDCFrameBuffer);
};

class CCubeMesh : public CMesh {
public:
	CCubeMesh(float fWidth = 4.0f, float fHeight = 4.0f, float fDepth = 4.0f);
	virtual ~CCubeMesh() {}
};

