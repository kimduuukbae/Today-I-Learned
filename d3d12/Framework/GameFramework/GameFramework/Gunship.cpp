#include "stdafx.h"
#include "Gunship.h"
#include "Scene.h"

Gunship::Gunship()
{

}

void Gunship::Init()
{
	LoadFrameHierarchyFromFile();

	mainRotor = frame->FindFrame("Rotor");
	tailRotor = frame->FindFrame("Back_Rotor");

	target = GetScene()->FindObject("Player");
}

void Gunship::Draw(ID3D12GraphicsCommandList* cmdList)
{
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

	float f{ std::acosf(Math::Dot(l, r)) };

	if (Math::Dot(r, a) > 0.0f)
		GetTransform()->RotateY(f);
	else
		GetTransform()->RotateY(-f);

	if (length > 100.0f)
		GetTransform()->Forward(100.0f * gt.DeltaTime());
}

void Gunship::LoadFrameHierarchyFromFile()
{
	FILE* pInFile{ nullptr };
	::fopen_s(&pInFile, "Models\\Gunship.bin", "rb");
	::rewind(pInFile);

	frame = Frame::LoadFrameHierarchyFromFile(pInFile);

	::fclose(pInFile);
}
