#include "stdafx.h"
#include "Bullet.h"
#include "GameplayStatics.h"
#include "TextureComponent.h"
#include "MeshComponent.h"

Bullet::Bullet()
{
}

void Bullet::Init()
{
	mesh = AddComponent<MeshComponent>();
	mesh->SetMesh(GameplayStatics::GetMesh("Sphere"));

	texture = AddComponent<TextureComponent>();
	texture->SetTexture(GameplayStatics::GetTexture("Textures\\bricks.dds"));

	SetLayer(0);
}

void Bullet::Draw(ID3D12GraphicsCommandList* cmdList)
{
	mesh->BindingResource(cmdList);
	texture->BindingResource(cmdList);
	cmdList->SetGraphicsRootConstantBufferView(0, GetTransform()->GetResourceAddress());
	mesh->Draw(cmdList);
}
void Bullet::Update(const GameTimer& gt)
{
	GetTransform()->Forward(500.0f * gt.DeltaTime());
}
