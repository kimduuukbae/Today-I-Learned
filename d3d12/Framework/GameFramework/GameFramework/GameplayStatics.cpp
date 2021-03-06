#include "stdafx.h"
#include "GameplayStatics.h"
#include "CameraComponent.h"
#include "ResourceManager.h"
#include "MeshManager.h"
#include "SceneManager.h"
#include "CollisionComponent.h"

float deltaTime;
float totalTime;

void GameplayStatics::SetMainCamera(CameraComponent* camComp)
{
	ResourceManager::GetInstance()->SetMainCamera(camComp);
}

bool GameplayStatics::IsInCamera(CollisionComponent* collision)
{
	return collision->IsOverlap(ResourceManager::GetInstance()->mainCam->GetFrustum());
}

ID3D12PipelineState* GameplayStatics::GetPSO(const std::string& name)
{
	return ResourceManager::GetInstance()->GetPSO(name);
}

void GameplayStatics::SetMainCameraFov(float fovY, float aspect, float nZ, float fZ)
{
	if (auto mainCam{ ResourceManager::GetInstance()->mainCam }; mainCam)
		mainCam->SetLens(fovY, aspect, nZ, fZ);
}

float GameplayStatics::GetAspectRatio()
{
	return D3DApp::GetApp()->AspectRatio();
}

Texture* GameplayStatics::GetTexture(const std::filesystem::path& p)
{
	return ResourceManager::GetInstance()->LoadTexture(p);
}

float GameplayStatics::GetDeltaTime()
{
	return deltaTime;
}

float GameplayStatics::GetTotalTime()
{
	return totalTime;
}

MeshBase* GameplayStatics::GetMesh(const std::string& name)
{
	return MeshManager::GetInstance()->GetMesh(name);
}

Scene* GameplayStatics::GetCurrentScene()
{
	return SceneManager::GetInstance()->mainScene;
}
