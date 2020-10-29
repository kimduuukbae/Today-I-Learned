#include "stdafx.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "Material.h"

void MeshComponent::SetMesh(Mesh* target)
{
	mesh = target;
}

void MeshComponent::SetMaterial(Material* mat)
{
	material = mat;
}

void MeshComponent::BindingResource(ID3D12GraphicsCommandList* cmdList)
{
	mesh->BindingResource(cmdList);
}

void MeshComponent::Draw(ID3D12GraphicsCommandList* cmdList)
{
	mesh->Draw(cmdList);
}
