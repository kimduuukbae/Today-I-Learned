#include "stdafx.h"
#include "Bullet.h"
#include "GameplayStatics.h"
#include "TextureComponent.h"
#include "MeshComponent.h"
#include "CollisionComponent.h"

Bullet::Bullet()
{
}

void Bullet::Init()
{
	mesh = AddComponent<MeshComponent>();
	mesh->SetMesh(GameplayStatics::GetMesh("Sphere"));

	texture = AddComponent<TextureComponent>();
	texture->SetTexture(GameplayStatics::GetTexture("Textures\\bricks.dds"));

	collision = AddComponent<CollisionComponent>();
	collision->SetRadius(10.0f);
	collision->AddCallbackFunction([this](CollisionComponent& other)
		{
			ProcessCollision(other);
		});

	SetLayer(0);
	SetName("Bullet");
}

void Bullet::Draw(ID3D12GraphicsCommandList* cmdList)
{
	if (IsActive()) {
		mesh->BindingResource(cmdList);
		texture->BindingResource(cmdList);
		cmdList->SetGraphicsRootConstantBufferView(0, GetTransform()->GetResourceAddress());
		mesh->Draw(cmdList);
	}
}
void Bullet::Update(const GameTimer& gt)
{
	if(IsActive())
		GetTransform()->Forward(500.0f * gt.DeltaTime());
}

void Bullet::ProcessCollision(CollisionComponent& other)
{
	DeActivate();
}
