#pragma once
#include "Mesh.h"

class BoxApp : public D3DApp{
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
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> cbvHeap{ nullptr };	//상수 버퍼 뷰

	std::unique_ptr<D3D::UploadBuffer<ObjectConstants>> objectCB{ nullptr };	// 오브젝트 상수 버퍼 ( matrix 담을 )
	std::unique_ptr<MeshGeometry> BoxGeometry{ nullptr };
	
	Microsoft::WRL::ComPtr<ID3DBlob> VSByteCode{ nullptr };
	Microsoft::WRL::ComPtr<ID3DBlob> PSByteCode{ nullptr };

	std::vector<D3D12_INPUT_ELEMENT_DESC> inputLayout{};

	Microsoft::WRL::ComPtr<ID3D12PipelineState> PSO{ nullptr };

	DirectX::XMFLOAT4X4 worldMatrix{ VectorMath::Identity4X4() };
	DirectX::XMFLOAT4X4 viewMatrix{ VectorMath::Identity4X4() };
	DirectX::XMFLOAT4X4 projMatrix{ VectorMath::Identity4X4() };

	float theta{ 1.5f * DirectX::XM_PI };
	float phi{ DirectX::XM_PIDIV4 };
	float radius{ 5.0f };

	POINT lastMousePos{};
};

