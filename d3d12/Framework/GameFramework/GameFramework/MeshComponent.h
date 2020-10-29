#pragma once

class Mesh;
class Material;

class MeshComponent
{
public:
	MeshComponent() = default;
	~MeshComponent() = default;

	void SetMesh(Mesh* target);
	void SetMaterial(Material* mat);

	void BindingResource(ID3D12GraphicsCommandList* cmdList);
	void Draw(ID3D12GraphicsCommandList* cmdList);
private:
	Mesh* mesh{ nullptr };
	Material* material{ nullptr };
};

