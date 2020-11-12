#pragma once

class MeshBase;
class Material;
struct Vertex;

class MeshComponent
{
public:
	MeshComponent();
	~MeshComponent();

	void SetMesh(MeshBase* target);
	void SetMaterial(Material* mat);

	void BindingResource(ID3D12GraphicsCommandList* cmdList);
	void Draw(ID3D12GraphicsCommandList* cmdList);
private:
	MeshBase* mesh{ nullptr };
	Material* material{ nullptr };
};

