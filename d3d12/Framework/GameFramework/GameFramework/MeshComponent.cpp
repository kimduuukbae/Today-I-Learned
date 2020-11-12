#include "stdafx.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "Material.h"

MeshComponent::MeshComponent()
{
}

MeshComponent::~MeshComponent()
{
}

void MeshComponent::SetMesh(MeshBase* target)
{
	mesh = target;
}

void MeshComponent::SetMaterial(Material* mat)
{
	material = mat;
}

void MeshComponent::BindingResource(ID3D12GraphicsCommandList* cmdList)
{
	if(mesh)
		mesh->BindingResource(cmdList);
}

void MeshComponent::Draw(ID3D12GraphicsCommandList* cmdList)
{
	if(mesh)
	mesh->Draw(cmdList);
}