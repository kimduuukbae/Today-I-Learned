#include "stdafx.h"
#include "GameplayStatics.h"
#include "CameraComponent.h"
#include "ResourceManager.h"
#include "MeshManager.h"
#include "SceneManager.h"


void GameplayStatics::SetMainCamera(CameraComponent* camComp)
{
	ResourceManager::GetInstance()->SetMainCamera(camComp);
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
	return nullptr;
}
