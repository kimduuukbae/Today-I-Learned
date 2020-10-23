#pragma once

struct Lookup
{
	DirectX::XMFLOAT3 lookup{};
};

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
	DirectX::XMFLOAT4X4 world{ VectorMath::Identity4X4() };
	DirectX::XMFLOAT4X4 texMat{ VectorMath::Identity4X4() };
};

struct Object {
	ObjectConstants objConst;
	std::size_t CBIndex{};
	D3D::Material* Mat;
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

	std::size_t TextureSRVIndex{};
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
	// 한 MeshGeometry 인스턴스의 한 Vertex/Index 버퍼에 여러개의 Geometry들을 담을 수 있다.
	// 부분 메시들을 개별적으로 그릴 수 있도록, 부분 메시 기하구조들을 컨테이너에 담는다.

	std::unordered_map<std::string, MeshData> Meshs{};

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView(const std::string& dataName) {
		D3D12_VERTEX_BUFFER_VIEW vbv{};
		vbv.BufferLocation = Meshs[dataName].VertexBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes = Meshs[dataName].VertexByteStride;
		vbv.SizeInBytes = Meshs[dataName].VertexBufferByteSize;

		return vbv;
	}

	D3D12_INDEX_BUFFER_VIEW IndexBufferView(const std::string& dataName) {
		D3D12_INDEX_BUFFER_VIEW ibv{
		.BufferLocation { Meshs[dataName].IndexBufferGPU->GetGPUVirtualAddress() },
		.SizeInBytes  { Meshs[dataName].IndexBufferByteSize },
		.Format  { Meshs[dataName].IndexFormat }
		};
		return ibv;
	}

	void DisposeUploaders() {
		for (auto& it : Meshs) {
			if (auto& p{ it.second.VertexBufferUploader }; p)
				p = nullptr;
			if (auto& p{ it.second.IndexBufferUploader }; p)
				p = nullptr;
		}
	}
};


std::pair<std::vector<Vertex>, std::vector<int>>  loadVertexFromFile(const std::string_view& fileName);
MeshData CreateBox(float width, float height, float depth, 
	ID3D12Device* device, ID3D12GraphicsCommandList* commandList);

MeshData CreateSphere(float radius, std::uint32_t stack, std::uint32_t slice,
	ID3D12Device* device, ID3D12GraphicsCommandList* commandList);

Texture CreateTexture(ID3D12Device* device, ID3D12GraphicsCommandList* commandList,
	const std::wstring_view& fileName, const std::string_view& name);