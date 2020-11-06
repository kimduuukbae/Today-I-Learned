#include "stdafx.h"
#include "Terrain.h"
#include "MeshComponent.h"
#include "MeshManager.h"
#include "TextureComponent.h"
#include "GameplayStatics.h"

Terrain::Terrain()
{

}

void Terrain::Init()
{
	mesh = AddComponent<MeshComponent>();
	mesh->SetMesh(MeshManager::GetInstance()->GetMesh("Landscape"));

	texture = AddComponent<TextureComponent>();
	texture->SetTexture(GameplayStatics::GetTexture("Textures\\Base_Texture.dds"));
	texture->AddTexture(GameplayStatics::GetTexture("Textures\\Detail_Texture.dds"));
	SetLayer(1);
}

void Terrain::Draw(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->SetPipelineState(GameplayStatics::GetPSO("Landscape"));

	mesh->BindingResource(cmdList);
	texture->BindingResource(cmdList);
	cmdList->SetGraphicsRootConstantBufferView(0, GetTransform()->GetResourceAddress());
	mesh->Draw(cmdList);
}