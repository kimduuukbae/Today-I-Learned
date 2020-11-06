#include "stdafx.h"
#include "GameScene.h"
#include "Player.h"
#include "Cube.h"
#include "SkySphere.h"
#include "Terrain.h"
#include "Billboard.h"

GameScene::GameScene()
{

}

GameScene::~GameScene()
{

}

void GameScene::Init()
{
	//SpawnObject<Cube>(DirectX::XMFLOAT3{ 0.0f, 0.0f, -10.0f }, DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f });
	SpawnObject<Player>(DirectX::XMFLOAT3{ 1440.0f, 234.0f, 320.0f }, DirectX::XMFLOAT3{ });
	SpawnObject<Terrain>(DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f }, DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f });
	SpawnObject<Billboard>(DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f }, DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f });
	SpawnObject<SkySphere>(DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f }, DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f });
}
