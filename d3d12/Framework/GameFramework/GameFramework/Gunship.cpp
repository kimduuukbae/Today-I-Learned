#include "stdafx.h"
#include "Gunship.h"
#include "Scene.h"
#include "CollisionComponent.h"
#include "Particle.h"
#include "TextureComponent.h"
#include "GameplayStatics.h"

Gunship::Gunship()
{

}

void Gunship::Init()
{
	LoadFrameHierarchyFromFile();

	mainRotor = frame->FindFrame("Rotor");
	tailRotor = frame->FindFrame("Back_Rotor");

	target = GetScene()->FindObject("Player");

	collision = AddComponent<CollisionComponent>();
	collision->SetRadius(6.0f);
	collision->AddCallbackFunction([this](CollisionComponent& other)
		{
			ProcessCollision(other);
		});
}

void Gunship::Draw(ID3D12GraphicsCommandList* cmdList)
{
	if (GameplayStatics::IsInCamera(collision))
		frame->Draw(cmdList);
}

void Gunship::Update(const GameTimer& gt)
{
	Super::Update(gt);

	mainRotor->GetLocalTransform() = Math::Multiply(
		DirectX::XMMatrixRotationY(DirectX::XMConvertToRadians(360.0f * 2.0f) * gt.DeltaTime()),
		mainRotor->GetLocalTransform());

	tailRotor->GetLocalTransform() = Math::Multiply(
		DirectX::XMMatrixRotationX(DirectX::XMConvertToRadians(360.0f * 4.0f) * gt.DeltaTime()),
		tailRotor->GetLocalTransform());

	frame->UpdateMeshMatrix(&GetTransform()->GetTransformDirect());

	DirectX::XMFLOAT3 tarDir{ Math::Subtract(target->GetTransform()->GetPosition(), GetTransform()->GetPosition()) };
	float length{ Math::Length(tarDir) };

	DirectX::XMFLOAT3 a{ Math::Vector3Normalize(tarDir) };

	DirectX::XMFLOAT3 l{ GetTransform()->GetLook() };
	DirectX::XMFLOAT3 r{ GetTransform()->GetRight() };

	const float f{ DirectX::XMConvertToDegrees(std::acosf(std::clamp(Math::Dot(l, a), -1.0f, 1.0f))) };

	if (Math::Dot(r, a) > 0.0f)
		GetTransform()->RotateY(f);
	else
		GetTransform()->RotateY(-f);

	if (length > 20.0f)
		GetTransform()->Forward(100.0f * gt.DeltaTime());
}

void Gunship::LoadFrameHierarchyFromFile()
{
	FILE* pInFile{ nullptr };
	::fopen_s(&pInFile, "Models\\Gunship.bin", "rb");
	::rewind(pInFile);

	frame = Frame::LoadFrameHierarchyFromFile(pInFile);

	auto f{ frame->FindFrame("Gunship") };
	f->GetComponent<TextureComponent>()->AddTexture(
		GameplayStatics::GetTexture("Model\\Textures\\1K_GunshipTXTR(Normal).dds"));

	::fclose(pInFile);
}

void Gunship::ProcessCollision(CollisionComponent& other)
{
	using namespace std;

	if (other.GetOwner()->GetName() == "Bullet"sv && 
		other.GetOwner()->IsActive()) {
		GetScene()->SpawnObject<Particle>(GetTransform()->GetPosition());
		Destroy();
	}
}
