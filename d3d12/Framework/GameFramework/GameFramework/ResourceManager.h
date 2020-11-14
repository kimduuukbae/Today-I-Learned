#pragma once

#include "Singleton.h"
#include "Common.h"
#include "Texture.h"

class CameraComponent;

class ResourceManager : public Singleton<ResourceManager>
{
public:
	ResourceManager() = default;
	virtual ~ResourceManager();

	void Init();
	void SetMainCamera(CameraComponent* camComp);
	ID3D12PipelineState* GetPSO(const std::string& name);

	Texture* LoadTexture(const std::filesystem::path& path);
	Texture* LoadTextureFromFile(FILE* file);

private:
	friend class SceneManager;
	friend class GameplayStatics;

	void CreateRootSignature();
	void CreatePSO();
	void CreateResources();
	void CreateShaderResourceView();

	void BindingResource(ID3D12GraphicsCommandList* cmdList);
	void ReleaseUploadBuffer();

	struct PassInfomation
	{
		DirectX::XMFLOAT4X4 viewMatrix;
		DirectX::XMFLOAT4X4 projMatrix;
		DirectX::XMFLOAT4X4 viewProj;
		DirectX::XMFLOAT3 eyePosition;
		float totalTime;
	};
	std::unique_ptr<Buffers::UploadBuffer<PassInfomation>> passCB{ nullptr };

	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> psos;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12RootSignature>> signature;

	std::unordered_map<std::wstring, std::unique_ptr<Texture>> textures;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap{ nullptr };

	class CameraComponent* mainCam{ nullptr };

	UINT srvIncSize{ 0 };
};

