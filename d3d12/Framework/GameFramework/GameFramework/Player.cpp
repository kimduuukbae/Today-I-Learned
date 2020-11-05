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

	auto [x, y, z] = GetTransform()->GetPosition();

	cameraComponent->SetPosition(x, y + 10.0f, z - 15.0f);
	GameplayStatics::SetMainCamera(cameraComponent.get());

	inputComponent = AddComponent<InputComponent>();
	inputComponent->BindInput('W', true, [this]() { UpKey(); });
	inputComponent->BindInput('S', true, [this]() { DownKey(); });
	inputComponent->BindInput('A', true, [this]() { LeftKey(); });
	inputComponent->BindInput('D', true, [this]() { RightKey(); });
	inputComponent->BindInput('I', true, [this]() { PitchUp(); });
	inputComponent->BindInput('K', true, [this]() { PitchDown(); });
	inputComponent->BindInput('J', true, [this]() { RotateLeft(); });
	inputComponent->BindInput('L', true, [this]() { RotateRight(); });
}

void Player::Draw(ID3D12GraphicsCommandList* cmdList)
{

}

void Player::LeftKey()
{
	cameraComponent->Strafe(-1000.0f * GameplayStatics::GetDeltaTime());
}

void Player::RightKey()
{
	cameraComponent->Strafe(1000.0f * GameplayStatics::GetDeltaTime());
}

void Player::UpKey()
{
	cameraComponent->Walk(1000.0f * GameplayStatics::GetDeltaTime());
}

void Player::DownKey()
{
	cameraComponent->Walk(-1000.0f * GameplayStatics::GetDeltaTime());
	auto [x, y, z] = GetTransform()->GetPosition();

	cameraComponent->SetPosition(x, y + 10.0f, z - 15.0f);
}

void Player::PitchUp()
{
	cameraComponent->Pitch(-3.14f * GameplayStatics::GetDeltaTime());
}

void Player::PitchDown()
{
	cameraComponent->Pitch(3.14f * GameplayStatics::GetDeltaTime());
}

void Player::RotateLeft()
{
	cameraComponent->RotateY(-3.14f * GameplayStatics::GetDeltaTime());
}

void Player::RotateRight()
{
	cameraComponent->RotateY(3.14f * GameplayStatics::GetDeltaTime());
}
