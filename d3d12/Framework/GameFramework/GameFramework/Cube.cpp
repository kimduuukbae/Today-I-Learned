#include "stdafx.h"
#include "Cube.h"
#include "MeshComponent.h"
#include "MeshManager.h"
#include "TextureComponent.h"
#include "GameplayStatics.h"

Cube::Cube()
{

}

void Cube::Init()
{
	mesh = AddComponent<MeshComponent>();
	mesh->SetMesh(MeshManager::GetInstance()->GetMesh("Cube"));

	texture = AddComponent<TextureComponent>();
	texture->SetTexture(GameplayStatics::GetTexture("Textures\\bricks.dds"));

	SetLayer(0);
}

void Cube::Draw(ID3D12GraphicsCommandList* cmdList)
{
	mesh->BindingResource(cmdList);
	texture->BindingResource(cmdList);
	cmdList->SetGraphicsRootConstantBufferView(0, GetTransform()->GetResourceAddress());
	mesh->Draw(cmdList);
}
