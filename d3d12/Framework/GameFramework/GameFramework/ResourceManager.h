#pragma once

#include "Singleton.h"

class ResourceManager : public Singleton<ResourceManager>
{
public:
	ResourceManager() = default;
	virtual ~ResourceManager();

	//Texture* LoadTexture(const std::string& filepath);

	void Init();

private:
	friend class SceneManager;

	void BindingResource(ID3D12GraphicsCommandList* cmdList);

	//std::unordered_map<std::string, std::unique_ptr<Texture>> textures;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> psos;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature{ nullptr };

	//static Texture* LoadTexture(const std::filesystem::path& path);

	void CreateRootSignature();
	void CreatePSO();
};

