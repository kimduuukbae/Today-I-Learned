#pragma once

class CVertex {
protected:
	XMFLOAT3 position{};	// 정점의 위치 벡터

public:
	CVertex() = default;
	CVertex(const XMFLOAT3& otherPos) : position{otherPos} { }
	~CVertex() = default;
};

class CDiffusedVertex : public CVertex {	// 즉 색상이 있는 Vertex
protected:
	XMFLOAT4 diffuse{};		// 정점의 Diffuse 색상

public:
	CDiffusedVertex() = default;
	CDiffusedVertex(float x, float y, float z, const XMFLOAT4& otherColor) {
		position = { x, y, z };
		diffuse = otherColor;
	}
	CDiffusedVertex(const XMFLOAT3& otherPos, const XMFLOAT4& otherColor) {
		position = otherPos;
		diffuse = otherColor;
	}
	~CDiffusedVertex() = default;
};

class CMesh{
public:
	CMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList);
	virtual ~CMesh();
	void AddRef() { ++ref; }
	void Release() { if (--ref <= 0) delete this; }
	void ReleaseUploadBuffers();
	virtual void Render(ID3D12GraphicsCommandList* commandList);
protected:
	ID3D12Resource* vertexBuffer{ nullptr };
	ID3D12Resource* vertexUploadBuffer{ nullptr };
	
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	D3D12_PRIMITIVE_TOPOLOGY primTopology{ D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };

	UINT slot{ 0 };
	UINT vertices{ 0 };
	UINT stride{ 0 };
	UINT offset{ 0 };
private:
	int	ref{};
};

class CTriangleMesh : public CMesh {
public:
	CTriangleMesh(ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	virtual ~CTriangleMesh() = default;
};
