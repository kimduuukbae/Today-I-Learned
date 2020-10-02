#pragma once
#include "Mesh.h"

class BoxApp : public D3DApp {
public:
	BoxApp(HINSTANCE hInstance);
	BoxApp(const BoxApp& rhs) = delete;
	BoxApp& operator=(const BoxApp& rhs) = delete;
	~BoxApp();

	virtual bool Initialize() override;

private:
	virtual void OnResize() override;
	virtual void Update(const GameTimer& gt) override;
	virtual void Draw(const GameTimer& gt) override;

	virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y) override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y) override;

	void BuildDescriptorHeaps();		// 상수버퍼 서술자 힙 생성
	void BuildConstantBuffers();		// Constant Buffer
	void BuildRootSignature();			// Root Signature
	void BuildShadersAndInputLayout();	// Shader Compile And Input Layout
	void BuildBoxGeometry();			// Box Mesh ( Geometry )
	void BuildPSO();					//Pipeline State Object

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature{ nullptr };	// 루트시그니쳐
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> cbvHeap{ nullptr };	//뷰들의 힙

	std::unique_ptr<D3D::UploadBuffer<ObjectConstants>> objectCB{ nullptr };	// 오브젝트 상수 버퍼 ( matrix 담을 )
	std::unique_ptr<D3D::UploadBuffer<MaterialConstants>> matCB{ nullptr };	// Material 상수 버퍼 ( matrix 담을 )
	std::unique_ptr<D3D::UploadBuffer<PassConstants>> passCB{ nullptr };	// pass 상수 버퍼 ( matrix 담을 )
	std::unique_ptr<D3D::UploadBuffer<Time>> timeCB{ nullptr };	// 시간값
	std::unique_ptr<MeshGeometry> BoxGeometry{ nullptr };

	Microsoft::WRL::ComPtr<ID3DBlob> VSByteCode{ nullptr };
	Microsoft::WRL::ComPtr<ID3DBlob> PSByteCode{ nullptr };
	Microsoft::WRL::ComPtr<ID3DBlob> OpaquePSByteCode{ nullptr };

	std::unordered_map<std::string, std::unique_ptr<D3D::Material>> mMaterials;
	std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3DBlob>> mShaders;
	std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;

	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout{};

	Microsoft::WRL::ComPtr<ID3D12PipelineState> PSO{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12PipelineState> opaquePSO{ nullptr };

	DirectX::XMFLOAT3 mEyePos = { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT4X4 mView{ VectorMath::Identity4X4() };
	DirectX::XMFLOAT4X4 mProj {VectorMath::Identity4X4() };

	PassConstants mMainPassCB;

	float theta{ 1.5f * DirectX::XM_PI };
	float phi{ DirectX::XM_PIDIV4 };
	float radius{ 5.0f };

	POINT lastMousePos{};
};

