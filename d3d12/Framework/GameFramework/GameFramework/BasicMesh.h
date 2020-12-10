#pragma once
#include "Mesh.h"

class CubeMesh : public Mesh
{
public:
	CubeMesh(float width, float height, float depth,
		ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	~CubeMesh();
};

class SphereMesh : public Mesh
{
public:
	SphereMesh(float radius, std::uint32_t stack, std::uint32_t slice,
		ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	~SphereMesh();
};

class Landscape : public MeshBase
{
public:
	Landscape(int width, int height, ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	~Landscape();

	virtual void BindingResource(ID3D12GraphicsCommandList* cmdList) override;
	virtual void Draw(ID3D12GraphicsCommandList* cmdList) override;

private:
	void PartitionMap(std::vector<BYTE>& v, int width, int height, int xStart, int zStart);

	std::vector<std::unique_ptr<CustomVertexMesh>> meshs;
};


// XZ 평면의 그리드
class GridXZ : public Mesh
{
public:
	GridXZ(uint32_t vertexX, uint32_t vertexY, float width, float depth,
	ID3D12Device* device, ID3D12GraphicsCommandList* commandList);
	~GridXZ();
};