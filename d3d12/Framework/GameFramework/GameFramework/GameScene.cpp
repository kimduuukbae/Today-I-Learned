#include "stdafx.h"
#include "GameScene.h"
#include "Player.h"
#include "Cube.h"

GameScene::GameScene()
{

}

GameScene::~GameScene()
{

}

void GameScene::Init()
{
	SpawnObject<Cube>(DirectX::XMFLOAT3{ 0.0f, 0.0f, -10.0f }, DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f });
	SpawnObject<Player>(DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f }, DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f });
}
