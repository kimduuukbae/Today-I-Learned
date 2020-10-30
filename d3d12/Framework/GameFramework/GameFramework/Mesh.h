#pragma once

#include "Common.h"

struct Vertex
{
	DirectX::XMFLOAT3 pos{};
	DirectX::XMFLOAT3 normal{};
	DirectX::XMFLOAT2 texCoord{};

	Vertex(float x, float y, float z,
		float nx, float ny, float nz,
		float tu, float tv) : pos{ x,y,z }, normal{ nx,ny,nz }, texCoord{ tu,tv }{}

	Vertex(DirectX::XMFLOAT3 tPos, DirectX::XMFLOAT3 tNorm, DirectX::XMFLOAT2 tTex)
		: pos{ tPos }, normal{ tNorm }, texCoord{ tTex } {}
	Vertex() = default;
};

// Dynamic Buffers
class MeshBase
{
public:
	MeshBase();
	virtual ~MeshBase();

	void ReleaseUploadBuffer();

protected:

	using Super = MeshBase;

	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView();
	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView();

	Microsoft::WRL::ComPtr<ID3D12Resource> vBuffer{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12Resource> iBuffer{ nullptr };

	Microsoft::WRL::ComPtr<ID3D12Resource> vUploadBuffer{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12Resource> iUploadBuffer{ nullptr };

	uint32_t vByteStride{};
	uint32_t vByteSize{};

	uint32_t iByteSize{};
	DXGI_FORMAT iFormat{};

	uint32_t iCount{};

	D3D12_PRIMITIVE_TOPOLOGY primTopology{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST };
};

class Mesh : public MeshBase
{
public:
	Mesh();
	virtual ~Mesh();

	void BindingResource(ID3D12GraphicsCommandList* cmdList);
	void Draw(ID3D12GraphicsCommandList* cmdList);
protected:
	using Super = Mesh;
};

