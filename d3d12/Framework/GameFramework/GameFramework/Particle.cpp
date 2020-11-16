#include "stdafx.h"
#include "Particle.h"
#include "GameplayStatics.h"
#include "MeshManager.h"
#include "MeshComponent.h"
#include "TextureComponent.h"
#include "Mesh.h"


Particle::Particle() : xSize{ 1.0f } { }

Particle::~Particle() { }

void Particle::Init()
{ 
	mesh = AddComponent<MeshComponent>();
	if (auto p = GameplayStatics::GetMesh("Point"); p) 
		mesh->SetMesh(p);
	else {
		std::vector<Vertex> v;
		v.emplace_back(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
		mesh->SetMesh(MeshManager::GetInstance()->CreateMesh(&v, nullptr,
			D3D_PRIMITIVE_TOPOLOGY_POINTLIST, "Point"));
	}
		
	texture = AddComponent<TextureComponent>();
	texture->SetTexture(GameplayStatics::GetTexture("Textures\\particle.dds"));

	SetLayer(4);
}

void Particle::Draw(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->SetPipelineState(GameplayStatics::GetPSO("Particle"));

	mesh->BindingResource(cmdList);
	texture->BindingResource(cmdList);
	cmdList->SetGraphicsRootConstantBufferView(0, GetTransform()->GetResourceAddress());
	mesh->Draw(cmdList);
}

void Particle::Update(const GameTimer& gt)
{
	xSize += gt.DeltaTime() * 30.0f;
	GetTransform()->SetTextureTransformSize(xSize, 1.0f);
	if (xSize >= 29.0f)
		Destroy();
}
