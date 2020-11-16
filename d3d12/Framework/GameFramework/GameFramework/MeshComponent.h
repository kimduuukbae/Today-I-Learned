#pragma once

#include "IComponent.h"

class MeshBase;
class Material;
struct Vertex;

class MeshComponent : public IComponent
{
public:
	MeshComponent();
	~MeshComponent();

	void SetMesh(MeshBase* target);
	void SetMaterial(Material* mat);

	bool IsBindingMesh();

	void BindingResource(ID3D12GraphicsCommandList* cmdList);
	void Draw(ID3D12GraphicsCommandList* cmdList);
private:
	MeshBase* mesh{ nullptr };
	Material* material{ nullptr };
};

