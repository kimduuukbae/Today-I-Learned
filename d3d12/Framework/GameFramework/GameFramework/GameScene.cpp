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

GameScene::GameScene()
{

}

GameScene::~GameScene()
{

}

void GameScene::Init()
{
	SpawnObject<Player>(DirectX::XMFLOAT3{ 1440.0f, 700.0f, 320.0f }, DirectX::XMFLOAT3{ });
	SpawnObject<SuperCobra>(DirectX::XMFLOAT3{ 2000.0f, 700.0f, 720.0f });
	SpawnObject<Gunship>(DirectX::XMFLOAT3{ 700.0f, 700.0f, 900.0f });
	
	SpawnObject<Terrain>();
	SpawnObject<Bullet>();
	SpawnObject<Water>(DirectX::XMFLOAT3{ 2032.0f, 280.0f, 2032.0f }, DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f });
	SpawnObject<Billboard>();
	SpawnObject<SkySphere>();
}
