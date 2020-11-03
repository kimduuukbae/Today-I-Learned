#include "stdafx.h"
#include "Player.h"
#include "MeshComponent.h"
#include "MeshManager.h"
#include "CameraComponent.h"
#include "Scene.h"
#include "GameplayStatics.h"
Player::Player()
{

}

void Player::Init()
{
	cameraComponent = AddComponent<CameraComponent>();
	cameraComponent->SetPosition(0.0f, 2.0f, -15.0f);
	GameplayStatics::SetMainCamera(cameraComponent.get());
}

void Player::Draw(ID3D12GraphicsCommandList* cmdList)
{
	//cameraComponent->Walk(-0.0001f);
}
