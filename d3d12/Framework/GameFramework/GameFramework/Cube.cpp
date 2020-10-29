#include "stdafx.h"
#include "Cube.h"
#include "MeshComponent.h"
#include "MeshManager.h"

Cube::Cube()
{

}

void Cube::Init()
{
	mesh = AddComponent<MeshComponent>();
	mesh->SetMesh(MeshManager::GetInstance()->GetMesh("Cube"));
}

void Cube::Draw(ID3D12GraphicsCommandList* cmdList)
{
	mesh->BindingResource(cmdList);
	cmdList->SetGraphicsRootConstantBufferView(0, GetTransform()->GetResourceAddress());
	mesh->Draw(cmdList);
}
