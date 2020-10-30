#pragma once

#include "Singleton.h"
#include "Common.h"

class CameraComponent;

class ResourceManager : public Singleton<ResourceManager>
{
public:
	ResourceManager() = default;
	virtual ~ResourceManager();

	//Texture* LoadTexture(const std::string& filepath);

	void Init();
	void SetMainCamera(CameraComponent* camComp);
	ID3D12PipelineState* GetPSO(const std::string& name);

private:

	struct PassInfomation
	{
		DirectX::XMFLOAT4X4 viewMatrix{ Math::Identity4x4() };
		DirectX::XMFLOAT4X4 projMatrix{ Math::Identity4x4() };
		DirectX::XMFLOAT4X4 viewProj{ Math::Identity4x4() };
	};

	friend class SceneManager;
	friend class GameplayStatics;

	void BindingResource(ID3D12GraphicsCommandList* cmdList);

	//std::unordered_map<std::string, std::unique_ptr<Texture>> textures;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> psos;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature{ nullptr };

	//static Texture* LoadTexture(const std::filesystem::path& path);

	void CreateRootSignature();
	void CreatePSO();
	void CreateResources();

	class CameraComponent* mainCam{ nullptr };
	std::unique_ptr<Buffers::UploadBuffer<PassInfomation>> passCB{ nullptr };
};

