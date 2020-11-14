#include "stdafx.h"
#include "Water.h"
#include "MeshComponent.h"
#include "TextureComponent.h"
#include "GameplayStatics.h"

Water::Water()
{
}

void Water::Init()
{
	mesh = AddComponent<MeshComponent>();
	mesh->SetMesh(GameplayStatics::GetMesh("GridXZ"));

	texture = AddComponent<TextureComponent>();
	texture->SetTexture(GameplayStatics::GetTexture("Textures\\water1.dds"));

	GetTransform()->SetTextureTransformSize(10.0f, 10.0f);

	SetLayer(4);
}

void Water::Draw(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->SetPipelineState(GameplayStatics::GetPSO("Blend"));

	mesh->BindingResource(cmdList);
	texture->BindingResource(cmdList);
	cmdList->SetGraphicsRootConstantBufferView(0, GetTransform()->GetResourceAddress());
	mesh->Draw(cmdList);
}
