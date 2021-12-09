#include "stdafx.h"
#include "Player.h"
#include "LagCameraComponent.h"
#include "GameplayStatics.h"
#include "InputComponent.h"
#include "TextureComponent.h"
#include "Scene.h"

Player::Player()
{

}

void Player::Init()
{
	cameraComponent = AddComponent<LagCameraComponent>();

	inputComponent = AddComponent<InputComponent>();
	inputComponent->BindInput('W', true, [this]() { UpKey(); });
	inputComponent->BindInput('S', true, [this]() { DownKey(); });
	inputComponent->BindInput('A', true, [this]() { LeftKey(); });
	inputComponent->BindInput('D', true, [this]() { RightKey(); });
	inputComponent->BindInput(VK_CONTROL, false, [this]() { CtrlKey(); });

	inputComponent->BindAxis(MK_LBUTTON, [this](float f) { MouseLeft(f); });

	cameraComponent->SetOffset(0.0f, 15.0f, -50.0f);
	cameraComponent->Pitch(15.0f);
	
	GameplayStatics::SetMainCamera(cameraComponent);

	SetName("Player");
}

void Player::Draw(ID3D12GraphicsCommandList* cmdList)
{

}

void Player::Update(const GameTimer& gt)
{
	Super::Update(gt);
}

void Player::LeftKey()
{
	GetTransform()->Right(-360.0f * GameplayStatics::GetDeltaTime());
	cameraComponent->Move(CommonMath::MultiplyScalar(GetTransform()->GetRight(), -360.0f * GameplayStatics::GetDeltaTime()));
}

void Player::RightKey()
{
	GetTransform()->Right(360.0f * GameplayStatics::GetDeltaTime());
	cameraComponent->Move(CommonMath::MultiplyScalar(GetTransform()->GetRight(), 360.0f * GameplayStatics::GetDeltaTime()));
}

void Player::UpKey()
{
	GetTransform()->Forward(360.0f * GameplayStatics::GetDeltaTime());
	cameraComponent->Move(CommonMath::MultiplyScalar(GetTransform()->GetLook(), 360.0f * GameplayStatics::GetDeltaTime()));
}

void Player::DownKey()
{
	GetTransform()->Forward(-360.0f * GameplayStatics::GetDeltaTime());
	cameraComponent->Move(CommonMath::MultiplyScalar(GetTransform()->GetLook(), -360.0f * GameplayStatics::GetDeltaTime()));
}

void Player::CtrlKey()
{

}

void Player::MouseLeft(float f)
{
	GetTransform()->RotateY(f * 540.0f * GameplayStatics::GetDeltaTime());
}