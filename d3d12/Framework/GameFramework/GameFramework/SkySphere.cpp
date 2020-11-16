#include "stdafx.h"
#include "SkySphere.h"
#include "MeshComponent.h"
#include "TextureComponent.h"
#include "GameplayStatics.h"

SkySphere::SkySphere()
{

}

void SkySphere::Init()
{
	mesh = AddComponent<MeshComponent>();
	mesh->SetMesh(GameplayStatics::GetMesh("Sphere"));

	texture = AddComponent<TextureComponent>();
	texture->SetTexture(GameplayStatics::GetTexture("Textures\\SkyBox_1.dds"));
	SetLayer(3);
}

void SkySphere::Draw(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->SetPipelineState(GameplayStatics::GetPSO("Skybox"));

	mesh->BindingResource(cmdList);
	texture->BindingResource(cmdList);
	cmdList->SetGraphicsRootConstantBufferView(0, GetTransform()->GetResourceAddress());
	mesh->Draw(cmdList);
}
