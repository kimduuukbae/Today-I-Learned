#include "stdafx.h"
#include "Player.h"
#include "CameraComponent.h"
#include "GameplayStatics.h"
#include "InputComponent.h"

Player::Player()
{

}

void Player::Init()
{
	cameraComponent = AddComponent<CameraComponent>();
	cameraComponent->SetPosition(0.0f, 2.0f, -15.0f);
	GameplayStatics::SetMainCamera(cameraComponent.get());

	inputComponent = AddComponent<InputComponent>();
	inputComponent->BindInput('W', true, [this]() { UpKey(); });
	inputComponent->BindInput('S', true, [this]() { DownKey(); });
	inputComponent->BindInput('W', true, [this]() { UpKey(); });
	inputComponent->BindInput('W', true, [this]() { UpKey(); });
}

void Player::Draw(ID3D12GraphicsCommandList* cmdList)
{

}

void Player::LeftKey()
{

}

void Player::RightKey()
{
}

void Player::UpKey()
{
	cameraComponent->Walk(10.0f * GameplayStatics::GetDeltaTime());
}

void Player::DownKey()
{
	cameraComponent->Walk(-10.0f * GameplayStatics::GetDeltaTime());
}
