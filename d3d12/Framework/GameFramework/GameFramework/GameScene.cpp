#include "stdafx.h"
#include "GameScene.h"
#include "Player.h"
#include "Cube.h"
#include "SkySphere.h"
#include "Terrain.h"
#include "Billboard.h"
#include "Water.h"

GameScene::GameScene()
{

}

GameScene::~GameScene()
{

}

void GameScene::Init()
{
	SpawnObject<Player>(DirectX::XMFLOAT3{ 1440.0f, 700.0f, 320.0f }, DirectX::XMFLOAT3{ });
	SpawnObject<Terrain>(DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f }, DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f });
	SpawnObject<Water>(DirectX::XMFLOAT3{ 2032.0f, 280.0f, 2032.0f }, DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f });
	SpawnObject<Billboard>(DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f }, DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f });
	SpawnObject<SkySphere>(DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f }, DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f });
}
