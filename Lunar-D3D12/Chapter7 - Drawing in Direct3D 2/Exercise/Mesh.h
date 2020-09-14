#pragma once

struct Vertex {
	DirectX::XMFLOAT3 pos{};
	DirectX::XMFLOAT4 color{};
};

struct ObjectConstants {
	DirectX::XMFLOAT4X4 worldViewProjMat{ VectorMath::Identity4X4() };
};

struct ObjectTime {
	float time{};
};

struct SubmeshGeometry {
	UINT IndexCount{};
	UINT StartIndexLocation{};
	UINT BaseVertexLocation{};

	DirectX::BoundingBox Bounds{};
};

struct MeshGeometry {
	std::string Name{};		// �̸����� ��ȸ�� �� �ֵ��� ��

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

	// �� MeshGeometry �ν��Ͻ��� �� Vertex/Index ���ۿ� �������� Geometry���� ���� �� �ִ�.
	// �κ� �޽õ��� ���������� �׸� �� �ֵ���, �κ� �޽� ���ϱ������� �����̳ʿ� ��´�.

	std::unordered_map<std::string_view, SubmeshGeometry> DrawArgs{};

	D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const {
		D3D12_VERTEX_BUFFER_VIEW vbv{};
		vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
		vbv.StrideInBytes = VertexByteStride;
		vbv.SizeInBytes = VertexBufferByteSize;

		return vbv;
	}

	D3D12_INDEX_BUFFER_VIEW IndexBufferView() const {
		D3D12_INDEX_BUFFER_VIEW ibv{};
		ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
		ibv.Format = IndexFormat;
		ibv.SizeInBytes = IndexBufferByteSize;

		return ibv;
	}

	void DisposeUploaders() {
		VertexBufferUploader = nullptr;
		IndexBufferUploader = nullptr;
	}
};

std::pair<std::vector<Vertex>, std::vector<int>>  loadVertexFromFile
(const std::string_view& fileName);