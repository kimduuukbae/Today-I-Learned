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

class MeshBase
{
public:
	MeshBase();
	virtual ~MeshBase();

	virtual void ReleaseUploadBuffer();
	virtual void BindingResource(ID3D12GraphicsCommandList* cmdList);
	virtual void Draw(ID3D12GraphicsCommandList* cmdList) = 0;
protected:

	using Super = MeshBase;

	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView();

	Microsoft::WRL::ComPtr<ID3D12Resource> vBuffer{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12Resource> vUploadBuffer{ nullptr };

	uint32_t vByteStride{ 0 };
	uint32_t vByteSize{ 0 };

	uint32_t vCount{ 0 };

	D3D12_PRIMITIVE_TOPOLOGY primTopology{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST };
};

class Mesh : public MeshBase
{
public:
	Mesh();
	virtual ~Mesh();

	virtual void BindingResource(ID3D12GraphicsCommandList* cmdList) override;
	virtual void Draw(ID3D12GraphicsCommandList* cmdList) override;
	virtual void ReleaseUploadBuffer() override;
protected:
	using Super = MeshBase;

	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView();

	Microsoft::WRL::ComPtr<ID3D12Resource> iBuffer{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12Resource> iUploadBuffer{ nullptr };

	uint32_t iByteSize{ 0 };
	uint32_t iCount{ 0 };
	DXGI_FORMAT iFormat{ DXGI_FORMAT_R32_UINT };
};

class FrameMesh : public MeshBase
{
public:
	FrameMesh();
	virtual ~FrameMesh();

	virtual void ReleaseUploadBuffer() override;
	virtual void BindingResource(ID3D12GraphicsCommandList* cmdList) override;
	virtual void Draw(ID3D12GraphicsCommandList* cmdList) override;
	void SetVertex(const std::vector<Vertex>& vertices);
	void AddSubMesh(const std::vector<uint32_t>& indices);

protected:
	using Super = MeshBase;

	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView(size_t where);

	struct IBufferCluster
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> subIBuffer{ nullptr };
		uint32_t iByteSize{ 0 };
		uint32_t iCount{ 0 };
	};

	std::vector<IBufferCluster> subIBuffers;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> subIUploadBuffers;

	DXGI_FORMAT iFormat{ DXGI_FORMAT_R32_UINT };
};

class CustomVertexMesh : public MeshBase
{
public:
	CustomVertexMesh();
	virtual ~CustomVertexMesh();

	virtual void BindingResource(ID3D12GraphicsCommandList* cmdList) override;
	virtual void ReleaseUploadBuffer() override;
	virtual void Draw(ID3D12GraphicsCommandList* cmdList) override;

	MeshBase* CreateMesh(
		std::vector<Vertex>* v,
		D3D12_PRIMITIVE_TOPOLOGY pTopology,
		const std::string& meshName);

protected:
	using Super = MeshBase;
};

class CustomIndexMesh : public CustomVertexMesh
{
public:
	CustomIndexMesh();
	virtual ~CustomIndexMesh();

	virtual void BindingResource(ID3D12GraphicsCommandList* cmdList);
	virtual void ReleaseUploadBuffer() override;
	virtual void Draw(ID3D12GraphicsCommandList* cmdList) override;

	MeshBase* CreateMesh(
		std::vector<Vertex>* v,
		std::vector<uint32_t>* i,
		D3D12_PRIMITIVE_TOPOLOGY pTopology,
		const std::string& meshName);

protected:
	using Super = CustomVertexMesh;

	D3D12_INDEX_BUFFER_VIEW GetIndexBufferView();

	Microsoft::WRL::ComPtr<ID3D12Resource> iBuffer{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12Resource> iUploadBuffer{ nullptr };

	uint32_t iByteSize{ 0 };
	uint32_t iCount{ 0 };
	DXGI_FORMAT iFormat{ DXGI_FORMAT_R32_UINT };
};