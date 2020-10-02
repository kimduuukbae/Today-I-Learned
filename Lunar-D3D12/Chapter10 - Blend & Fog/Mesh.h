#pragma once

struct Vertex {
	DirectX::XMFLOAT3 pos{};
	DirectX::XMFLOAT3 normal{};
	DirectX::XMFLOAT3 tangentU{};
	DirectX::XMFLOAT2 texCoord{};

	Vertex(float x, float y, float z,
		float nx, float ny, float nz,
		float tx, float ty, float tz,
		float tu, float tv) : pos{ x,y,z }, normal{ nx,ny,nz }, tangentU{ tx, ty, tz }, texCoord{ tu,tv }{}
	Vertex() = default;	
};

struct ObjectConstants {
	DirectX::XMFLOAT4X4 worldViewProjMat{ VectorMath::Identity4X4() };
	DirectX::XMFLOAT4X4 texMat{ VectorMath::Identity4X4() };
};

struct Time {
	float time{};
};

struct MaterialConstants {
	DirectX::XMFLOAT4 DiffuseAlbedo{ 1.0f, 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT3 FresnelR0{ 0.01f, 0.01f, 0.01f };
	float Roughness{ 0.25f };

	DirectX::XMFLOAT4X4 MatTransform{ VectorMath::Identity4X4() };
};

struct Light {
	DirectX::XMFLOAT3 Strength { 0.5f, 0.5f, 0.5f };
	float FalloffStart{ 1.0f };                          // point/spot light only
	DirectX::XMFLOAT3 Direction { 0.0f, -1.0f, 0.0f };// directional/spot light only
	float FalloffEnd{ 10.0f };                           // point/spot light only
	DirectX::XMFLOAT3 Position { 0.0f, 0.0f, 0.0f };  // point/spot light only
	float SpotPower{ 64.0f };                            // spot light only
};

struct SubmeshGeometry {
	UINT IndexCount{};
	UINT StartIndexLocation{};
	UINT BaseVertexLocation{};

	DirectX::BoundingBox Bounds{};
};

struct PassConstants
{
	DirectX::XMFLOAT4X4 View = VectorMath::Identity4X4();
	DirectX::XMFLOAT4X4 InvView = VectorMath::Identity4X4();
	DirectX::XMFLOAT4X4 Proj = VectorMath::Identity4X4();
	DirectX::XMFLOAT4X4 InvProj = VectorMath::Identity4X4();
	DirectX::XMFLOAT4X4 ViewProj = VectorMath::Identity4X4();
	DirectX::XMFLOAT4X4 InvViewProj = VectorMath::Identity4X4();
	DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
	float cbPerObjectPad1 = 0.0f;
	DirectX::XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
	DirectX::XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };
	float NearZ = 0.0f;
	float FarZ = 0.0f;
	float TotalTime = 0.0f;
	float DeltaTime = 0.0f;

	DirectX::XMFLOAT4 AmbientLight = { 0.0f, 0.0f, 0.0f, 1.0f };

	// Indices [0, NUM_DIR_LIGHTS) are directional lights;
	// indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
	// indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
	// are spot lights for a maximum of MaxLights per object.
	Light Lights[16];
};

struct Texture {
	std::string Name{};

	std::wstring FileName{};

	Microsoft::WRL::ComPtr<ID3D12Resource> Resource{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap{ nullptr };
};

struct MeshData {
	Microsoft::WRL::ComPtr<ID3DBlob> VertexBufferCPU{ nullptr };
	Microsoft::WRL::ComPtr<ID3DBlob> IndexBufferCPU{ nullptr };

	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferGPU{ nullptr };

	Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferUploader{ nullptr };

	UINT VertexByteStride{};
	UINT VertexBufferByteSize{};
	DXGI_FORMAT IndexFormat{ DXGI_FORMAT_R16_UINT };
	UINT IndexBufferByteSize{};

	UINT IndexCount{};
	UINT VertexCount{};
};

struct MeshGeometry {
	std::string Name{};		// 이름으로 조회할 수 있도록 함
	MeshData Data{};
	// 한 MeshGeometry 인스턴스의 한 Vertex/Index 버퍼에 여러개의 Geometry들을 담을 수 있다.
	// 부분 메시들을 개별적으로 그릴 수 있도록, 부분 메시 기하구조들을 컨테이너에 담는다.

	std::unordered_map<std::string_view, SubmeshGeometry> DrawArgs{};

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const {
		D3D12_VERTEX_BUFFER_VIEW vbv{};
		vbv.BufferLocation = Data.VertexBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes = Data.VertexByteStride;
		vbv.SizeInBytes = Data.VertexBufferByteSize;

		return vbv;
	}

	D3D12_INDEX_BUFFER_VIEW IndexBufferView() const {
		D3D12_INDEX_BUFFER_VIEW ibv{};
		ibv.BufferLocation = Data.IndexBufferGPU->GetGPUVirtualAddress();
		ibv.Format = Data.IndexFormat;
		ibv.SizeInBytes = Data.IndexBufferByteSize;

		return ibv;
	}

	void DisposeUploaders() {
		Data.VertexBufferUploader = nullptr;
		Data.IndexBufferUploader = nullptr;
	}
};


std::pair<std::vector<Vertex>, std::vector<int>>  loadVertexFromFile(const std::string_view& fileName);
MeshData CreateBox(float width, float height, float depth, 
	ID3D12Device* device, ID3D12GraphicsCommandList* commandList);