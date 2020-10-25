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
