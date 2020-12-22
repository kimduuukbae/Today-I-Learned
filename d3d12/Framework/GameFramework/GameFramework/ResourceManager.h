#pragma once

#include "Singleton.h"
#include "Common.h"
#include "Texture.h"
#include "ShadowMap.h"

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

	void PreProcessing(ID3D12GraphicsCommandList* cmdList);

	struct Light
	{
		DirectX::XMFLOAT3 strength{ 0.5f, 0.5f, 0.5f };
		float pad1;
		DirectX::XMFLOAT3 direction{ 0.0f, -1.0f, 0.0f };
		float pad2;
	};

	struct PassInfomation
	{
		DirectX::XMFLOAT4X4 viewMatrix;
		DirectX::XMFLOAT4X4 projMatrix;
		DirectX::XMFLOAT4X4 viewProj;
		DirectX::XMFLOAT4X4 shadowTransform;
		DirectX::XMFLOAT3 eyePosition;
		float totalTime;
		Light light[3];
		DirectX::XMFLOAT4 ambient;
	};

	DirectX::XMFLOAT4X4 st;

	std::unique_ptr<Buffers::UploadBuffer<PassInfomation>> passCB{ nullptr };
	std::unique_ptr<Buffers::UploadBuffer<PassInfomation>> shadowCB{ nullptr };

	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> psos;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12RootSignature>> signature;

	std::unordered_map<std::wstring, std::unique_ptr<Texture>> textures;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap{ nullptr };

	class CameraComponent* mainCam{ nullptr };

	std::unique_ptr<ShadowMap> shadowMap;

	UINT srvIncSize{ 0 };
};

