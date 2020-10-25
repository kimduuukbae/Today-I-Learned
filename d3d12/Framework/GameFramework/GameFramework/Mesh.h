#pragma once

#include "Common.h"

/*
	하드웨어 인스턴싱을 한다고 하면 Mesh 안에 상수버퍼가 있는게 맞음

*/

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

protected:

	using Super = MeshBase;

	void Init();
	void ReleaseUploadBuffer();
	void IncreaseConstantBuffer();
	void Assign(size_t count);

	Microsoft::WRL::ComPtr<ID3D12Resource> vBuffer{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12Resource> iBuffer{ nullptr };

	Microsoft::WRL::ComPtr<ID3D12Resource> vUploadBuffer{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12Resource> iUploadBuffer{ nullptr };

	std::unique_ptr<Buffers::UploadBuffer<DirectX::XMFLOAT4X4>> cBuffer;

	size_t usedCount{};
	size_t maxCount{ 1000 };

	uint32_t vByteStride{};
	uint32_t vByteSize{};

	uint32_t iByteSize{};
	DXGI_FORMAT iFormat{};
};

class Mesh : public MeshBase
{
public:
	Mesh();
	virtual ~Mesh();
	void CopyData(const DirectX::XMFLOAT4X4& world);
protected:
	using Super = Mesh;
	size_t curCount{};
};

