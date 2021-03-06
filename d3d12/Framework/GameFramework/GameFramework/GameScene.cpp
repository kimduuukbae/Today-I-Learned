#include "stdafx.h"
#include "GameScene.h"
#include "Player.h"
#include "Cube.h"
#include "SkySphere.h"
#include "Terrain.h"
#include "Billboard.h"
#include "Water.h"
#include "Bullet.h"
#include "SuperCobra.h"
#include "Gunship.h"
#include "Particle.h"

GameScene::GameScene()
{

}

GameScene::~GameScene()
{

}

void GameScene::Init()
{
	SpawnObject<Player>(DirectX::XMFLOAT3{ 1440.0f, 700.0f, 320.0f }, DirectX::XMFLOAT3{ });
	for (int i = 0; i < 10; ++i) {
		SpawnObject<SuperCobra>(DirectX::XMFLOAT3{ 1000.0f + static_cast<float>(i * 50), 700.0f, 720.0f });
		SpawnObject<Gunship>(DirectX::XMFLOAT3{ 700.0f, 700.0f , 900.0f + static_cast<float>(i * 50) });
	}
	
	SpawnObject<Terrain>();
	SpawnObject<Bullet>()->Destroy();
	SpawnObject<Particle>()->Destroy();
	SpawnObject<Water>(DirectX::XMFLOAT3{ 2032.0f, 280.0f, 2032.0f }, DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f });
	SpawnObject<Billboard>();
	SpawnObject<SkySphere>();
}
