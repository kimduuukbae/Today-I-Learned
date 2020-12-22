#include "stdafx.h"
#include "Player.h"
#include "LagCameraComponent.h"
#include "GameplayStatics.h"
#include "InputComponent.h"
#include "TextureComponent.h"
#include "Scene.h"
#include "Bullet.h"

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

	LoadFrameHierarchyFromFile();

	mainRotor = frame->FindFrame("Top_Rotor");
	tailRotor = frame->FindFrame("Tail_Rotor");

	cameraComponent->SetOffset(0.0f, 15.0f, -50.0f);
	cameraComponent->Pitch(15.0f);
	cameraComponent->SetPosition(frame->GetTransform()->GetPosition());
	
	GameplayStatics::SetMainCamera(cameraComponent);

	SetName("Player");
}

void Player::Draw(ID3D12GraphicsCommandList* cmdList)
{
	frame->Draw(cmdList);
}

void Player::Update(const GameTimer& gt)
{
	Super::Update(gt);

	mainRotor->GetLocalTransform() = Math::Multiply(
		DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(360.0f * 2.0f) * gt.DeltaTime()),
		mainRotor->GetLocalTransform());

	tailRotor->GetLocalTransform() = Math::Multiply(
		DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(360.0f * 4.0f) * gt.DeltaTime()),
		tailRotor->GetLocalTransform());

	frame->UpdateMeshMatrix(&GetTransform()->GetTransformDirect());
}

void Player::LoadFrameHierarchyFromFile()
{
	FILE* pInFile{ nullptr };
	::fopen_s(&pInFile, "Models\\Mi24.bin", "rb");
	::rewind(pInFile);

	frame = Frame::LoadFrameHierarchyFromFile(pInFile);

	::fclose(pInFile);

	auto f{ frame->FindFrame("Mi24") };
	f->GetComponent<TextureComponent>()->AddTexture(
		GameplayStatics::GetTexture("Model\\Textures\\1K_Mi24_TXTR(Normal).dds"));
}

void Player::LoadMaterials(FILE* pInFile)
{

}

void Player::LeftKey()
{
	GetTransform()->Right(-360.0f * GameplayStatics::GetDeltaTime());
	cameraComponent->Move(Math::MultiplyScalar(GetTransform()->GetRight(), -360.0f * GameplayStatics::GetDeltaTime()));
}

void Player::RightKey()
{
	GetTransform()->Right(360.0f * GameplayStatics::GetDeltaTime());
	cameraComponent->Move(Math::MultiplyScalar(GetTransform()->GetRight(), 360.0f * GameplayStatics::GetDeltaTime()));
}

void Player::UpKey()
{
	GetTransform()->Forward(360.0f * GameplayStatics::GetDeltaTime());
	cameraComponent->Move(Math::MultiplyScalar(GetTransform()->GetLook(), 360.0f * GameplayStatics::GetDeltaTime()));
}

void Player::DownKey()
{
	GetTransform()->Forward(-360.0f * GameplayStatics::GetDeltaTime());
	cameraComponent->Move(Math::MultiplyScalar(GetTransform()->GetLook(), -360.0f * GameplayStatics::GetDeltaTime()));
}

void Player::CtrlKey()
{
	TransformComponent* t{ GetTransform() };

	auto ptr{ GetScene()->SpawnObject<Bullet>(t->GetPosition()) };
	ptr->GetTransform()->SetBasisVector(t->GetRight(), t->GetUp(), t->GetLook());
}

void Player::MouseLeft(float f)
{
	GetTransform()->RotateY(f * 540.0f * GameplayStatics::GetDeltaTime());
}