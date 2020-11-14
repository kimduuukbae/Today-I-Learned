#pragma once

class GameplayStatics
{
public:
	static void SetMainCamera(class CameraComponent* camComp);
	static ID3D12PipelineState* GetPSO(const std::string& name);
	/// <summary>
	///	If it's not pointing to Main Camera that will be crash 
	/// </summary>
	/// <param name="fovY"></param>
	/// <param name="aspect"></param>
	/// <param name="nZ"></param>
	/// <param name="fZ"></param>
	static void SetMainCameraFov(float fovY, float aspect, float nZ, float fZ);

	static float GetAspectRatio();
	static class Texture* GetTexture(const std::filesystem::path& p);
	static float GetDeltaTime();
	static float GetTotalTime();
	static class MeshBase* GetMesh(const std::string& name);
};

