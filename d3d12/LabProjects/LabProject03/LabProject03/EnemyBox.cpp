#include "stdafx.h"
#include "EnemyBox.h"
#include <random>

CEnemyBox::CEnemyBox(){
	XMVECTOR minVect{ XMVectorSet(-2.0f, -2.0f, -2.0f, 1.0f) };
	XMVECTOR maxVect{ XMVectorSet(2.0f, 2.0f, 2.0f, 1.0f) };

	BoundingBox::CreateFromPoints(boundBox, minVect, maxVect);
}

CEnemyBox::~CEnemyBox(){
	for (auto& i : particles)
		delete i;
}

void CEnemyBox::SetParticles(CCubeMesh* mesh, DWORD dwColor, int particleCount){
	std::random_device rd{};
	std::uniform_real_distribution<float> rotAxis{ 0.0f, 1.0f };
	std::uniform_real_distribution<float> movDir{ -1.0f, 1.0f };

	particles.reserve(particleCount);
	for (int i = 0; i < particleCount; ++i) {
		CGameObject* particle{ new CGameObject };
		particle->SetMesh(mesh);
		particle->SetColor(dwColor);
		particle->SetPosition(this->GetPosition());
		particle->SetRotationAxis(XMFLOAT3{ rotAxis(rd), rotAxis(rd), rotAxis(rd) });
		particle->SetRotationSpeed(90.0f);
		particle->SetMovingDirection(XMFLOAT3{ movDir(rd), movDir(rd), movDir(rd) });
		particle->SetMovingSpeed(10.0f);
		particles.push_back(particle);
	}
}

void CEnemyBox::SetLive(bool bFlags){
	bLive = bFlags;
	if(!bLive)
		for (auto& i : particles)
			i->SetPosition(GetPosition());
}

void CEnemyBox::Animate(float fElapsedTime){
	if (bLive) 
		CGameObject::Animate(fElapsedTime);
	else {
		fDieTime += fElapsedTime;
		if (fDieTime < 5.0f) {
			for (auto& i : particles)
				i->Animate(fElapsedTime);
		}
		else {
			fDieTime = 0.0f;
			SetLive(true);
		}
	}
}

void CEnemyBox::Render(HDC hDCFrameBuffer, CCamera* pCamera){
	if (bLive)
		CGameObject::Render(hDCFrameBuffer, pCamera);
	else 
		for (auto& i : particles)
			i->Render(hDCFrameBuffer, pCamera);
}

BoundingBox CEnemyBox::getCollisionBox(){
	BoundingBox curColl{ boundBox };
	curColl.Transform(curColl, XMLoadFloat4x4(&worldMatrix));
	return curColl;
}
