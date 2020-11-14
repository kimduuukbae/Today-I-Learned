#include "stdafx.h"
#include "BasicMesh.h"
#include <fstream>

CubeMesh::CubeMesh(float width, float height, float depth,
	ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	std::array<Vertex, 24> v{};
	float halfWidth{ 0.5f * width };
	float halfHeight{ 0.5f * height };
	float halfDepth{ 0.5f * depth };

	v[0] = Vertex(-halfWidth, -halfHeight, -halfDepth, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f);
	v[1] = Vertex(-halfWidth, +halfHeight, -halfDepth, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f);
	v[2] = Vertex(+halfWidth, +halfHeight, -halfDepth, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f);
	v[3] = Vertex(+halfWidth, -halfHeight, -halfDepth, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f);

	// Fill in the back face vertex data.
	v[4] = Vertex(-halfWidth, -halfHeight, +halfDepth, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
	v[5] = Vertex(+halfWidth, -halfHeight, +halfDepth, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f);
	v[6] = Vertex(+halfWidth, +halfHeight, +halfDepth, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	v[7] = Vertex(-halfWidth, +halfHeight, +halfDepth, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f);

	// Fill in the top face vertex data.
	v[8] = Vertex(-halfWidth, +halfHeight, -halfDepth, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f);
	v[9] = Vertex(-halfWidth, +halfHeight, +halfDepth, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f);
	v[10] = Vertex(+halfWidth, +halfHeight, +halfDepth, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f);
	v[11] = Vertex(+halfWidth, +halfHeight, -halfDepth, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the bottom face vertex data.
	v[12] = Vertex(-halfWidth, -halfHeight, -halfDepth, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f);
	v[13] = Vertex(+halfWidth, -halfHeight, -halfDepth, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f);
	v[14] = Vertex(+halfWidth, -halfHeight, +halfDepth, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f);
	v[15] = Vertex(-halfWidth, -halfHeight, +halfDepth, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f);

	// Fill in the left face vertex data.
	v[16] = Vertex(-halfWidth, -halfHeight, +halfDepth, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[17] = Vertex(-halfWidth, +halfHeight, +halfDepth, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[18] = Vertex(-halfWidth, +halfHeight, -halfDepth, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[19] = Vertex(-halfWidth, -halfHeight, -halfDepth, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	// Fill in the right face vertex data.
	v[20] = Vertex(+halfWidth, -halfHeight, -halfDepth, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	v[21] = Vertex(+halfWidth, +halfHeight, -halfDepth, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	v[22] = Vertex(+halfWidth, +halfHeight, +halfDepth, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f);
	v[23] = Vertex(+halfWidth, -halfHeight, +halfDepth, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

	std::array<std::uint32_t, 36> i{};

	// Fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// Fill in the back face index data
	i[6] = 4; i[7] = 5; i[8] = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// Fill in the top face index data
	i[12] = 8; i[13] = 9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// Fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// Fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// Fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	const UINT vbSize{ sizeof(Vertex) * static_cast<UINT>(v.size()) };
	const UINT ibSize{ sizeof(std::uint32_t) * static_cast<UINT>(i.size()) };

	//vertex
	vBuffer = Buffers::CreateDefaultBuffer(device, commandList,
		v.data(), vbSize, vUploadBuffer);
	iBuffer = Buffers::CreateDefaultBuffer(device, commandList,
		i.data(), ibSize, iUploadBuffer);

	vByteSize = vbSize;
	vByteStride = sizeof(Vertex);

	iByteSize = ibSize;
	iFormat = DXGI_FORMAT_R32_UINT; 
	// Index Format 의 크기는 총바이트의 크기임, 한 요소의 크기가 아님.

	iCount = static_cast<UINT>(i.size());
}

CubeMesh::~CubeMesh(){}

SphereMesh::SphereMesh(float radius, std::uint32_t stack, std::uint32_t slice,
	ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	Vertex topVertex(0.0f, +radius, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, 0.0f);
	Vertex bottomVertex(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f);

	std::vector<Vertex> vertices{};
	std::vector<std::uint32_t> indices{};

	vertices.push_back(topVertex);

	float phiStep = DirectX::XM_PI / stack;
	float thetaStep = 2.0f * DirectX::XM_PI / slice;

	// Compute vertices for each stack ring (do not count the poles as rings).
	for (uint32_t i = 1; i <= stack - 1; ++i)
	{
		float phi = i * phiStep;

		// Vertices of ring.
		for (uint32_t j = 0; j <= slice; ++j)
		{
			float theta = j * thetaStep;

			Vertex v;

			// spherical to cartesian
			v.pos.x = radius * sinf(phi) * cosf(theta);
			v.pos.y = radius * cosf(phi);
			v.pos.z = radius * sinf(phi) * sinf(theta);

			DirectX::XMVECTOR p = XMLoadFloat3(&v.pos);
			XMStoreFloat3(&v.normal, DirectX::XMVector3Normalize(p));

			v.texCoord.x = theta / DirectX::XM_2PI;
			v.texCoord.y = phi / DirectX::XM_PI;

			vertices.push_back(v);
		}
	}

	vertices.push_back(bottomVertex);

	for (uint32_t i = 1; i <= slice; ++i)
	{
		indices.push_back(0);
		indices.push_back(i + 1);
		indices.push_back(i);
	}

	uint32_t baseIndex = 1;
	uint32_t ringVertexCount = slice + 1;
	for (uint32_t i = 0; i < stack - 2; ++i)
	{
		for (uint32_t j = 0; j < slice; ++j)
		{
			indices.push_back(baseIndex + i * ringVertexCount + j);
			indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);

			indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
			indices.push_back(baseIndex + i * ringVertexCount + j + 1);
			indices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
		}
	}

	std::uint32_t southPoleIndex = (uint32_t)vertices.size() - 1;

	baseIndex = southPoleIndex - ringVertexCount;

	for (uint32_t i = 0; i < slice; ++i)
	{
		indices.push_back(southPoleIndex);
		indices.push_back(baseIndex + i);
		indices.push_back(baseIndex + i + 1);
	}

	const UINT vbSize{ static_cast<UINT>(vertices.size()) * sizeof(Vertex) };
	const UINT ibSize{ static_cast<UINT>(indices.size()) * sizeof(uint32_t) };

	vBuffer = Buffers::CreateDefaultBuffer(device, commandList,
		vertices.data(), vbSize, vUploadBuffer);
	iBuffer = Buffers::CreateDefaultBuffer(device, commandList,
		indices.data(), ibSize, iUploadBuffer);

	vByteSize = vbSize;
	vByteStride = sizeof(Vertex);

	iFormat = DXGI_FORMAT_R32_UINT;
	iByteSize = ibSize;

	iCount = static_cast<UINT>(indices.size());
}

SphereMesh::~SphereMesh(){}

Landscape::Landscape(int width, int height, ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	std::ifstream is{"Textures\\HeightMap.raw", std::ios::binary};
	std::vector<BYTE> heightMap(width * height);
	is.read(reinterpret_cast<char*>(&heightMap[0]), heightMap.size() - 1);

	std::function<float(int, int)> GetHeight
	{
		[&heightMap, &width](int x, int z) {
			return heightMap[static_cast<size_t>(x) + static_cast<size_t>((z * width))] * 4.0f;
		}
	};

	std::function<DirectX::XMFLOAT3(int, int)> GetNormal
	{
		[&heightMap, &width, &height](int x, int z) {
			if ((x < 0.0f) || (z < 0.0f) || (x >= width) || (z >= height)) return DirectX::XMFLOAT3{0.0f, 1.0f, 0.0f};

			size_t heightMapIndex{ static_cast<size_t>(x + (z * width)) };
			size_t xHeightMapAdd{ static_cast<size_t>((x < (width - 1)) ? 1 : -1) };
			size_t zHeightMapAdd{ static_cast<size_t>((z < (height - 1)) ? width : -width) };
			float y1{ static_cast<float>(heightMap[heightMapIndex] * 4.0f) };
			float y2{ static_cast<float>(heightMap[heightMapIndex + xHeightMapAdd] * 4.0f) };
			float y3{ static_cast<float>(heightMap[heightMapIndex + zHeightMapAdd] * 4.0f) };

			DirectX::XMFLOAT3 edge1 = {0.0f, y3 - y1, 16.0f};
			DirectX::XMFLOAT3 edge2 = {16.0f, y2 - y1, 0.0f};
			DirectX::XMFLOAT3 norm = Math::CrossProduct(edge1, edge2, true);

			return norm;
		}
	};

	std::vector<Vertex> vertices;
	vertices.reserve(static_cast<size_t>(width) * static_cast<size_t>(height));

	int zStart{}, xStart{};
	float cHeight{}, minHeight{ FLT_MAX }, maxHeight{ -FLT_MAX };
	for (int i = 0, z = zStart; z < (zStart + height); ++z)
	{
		for (int x = xStart; x < (xStart + width); ++x, ++i)
		{
			cHeight = GetHeight(x, z);
			DirectX::XMFLOAT3 norm{ GetNormal(x, z) };
			Vertex v
			{ 
				static_cast<float>(x) * 16.0f, cHeight, static_cast<float>(z) * 16.0f,
				norm.x, norm.y, norm.z,
				static_cast<float>(x) / static_cast<float>(width - 1),
				static_cast<float>(height - 1 - z) / static_cast<float>(height - 1) 
			};
			vertices.emplace_back(v);
			if (cHeight < minHeight) minHeight = cHeight;
			if (cHeight > maxHeight) maxHeight = cHeight;
		}
	}

	std::vector<uint32_t> indices;
	indices.reserve(static_cast<size_t>(((width * 2) * (height - 1)) + ((height - 1) - 1)));

	for (int j = 0, z = 0; z < height - 1; z++)
	{
		if ((z % 2) == 0)
		{
			for (int x = 0; x < width; x++)
			{
				if ((x == 0) && (z > 0))
					indices.push_back(x + (z * width));

				indices.push_back(x + (z * width));
				indices.push_back((x + (z * width)) + width);
			}
		}
		else
		{
			for (int x = width - 1; x >= 0; x--)
			{
				if (x == (width - 1))
					indices.push_back(x + (z * width));

				indices.push_back(x + (z * width));
				indices.push_back((x + (z * width)) + width);
			}
		}
	}

	const UINT vbSize{ static_cast<UINT>(vertices.size()) * sizeof(Vertex) };
	const UINT ibSize{ static_cast<UINT>(indices.size()) * sizeof(uint32_t) };

	vBuffer = Buffers::CreateDefaultBuffer(device, commandList,
		vertices.data(), vbSize, vUploadBuffer);
	iBuffer = Buffers::CreateDefaultBuffer(device, commandList,
		indices.data(), ibSize, iUploadBuffer);

	vByteSize = vbSize;
	vByteStride = sizeof(Vertex);

	iFormat = DXGI_FORMAT_R32_UINT;
	iByteSize = ibSize;

	iCount = static_cast<UINT>(indices.size());

	primTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
}

Landscape::~Landscape()
{
}

GridXZ::GridXZ(uint32_t vertexX, uint32_t vertexY, float width, float depth, ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
{
	const size_t vertexCount{ static_cast<size_t>(vertexX) * vertexY };
	// 한칸이 삼각형 두 개, 만약 10칸을 만든다면, 9칸짜리 vertex가 필요함 ( 중복 )
	// 즉 vertexX - 1 * vertexY - 1 개의 vertex가 필요한데,
	// 삼각형 두 개이므로 vertexX - 1 * vertexY - 1 * 2 
	const size_t faceCount{ static_cast<size_t>(vertexX - 1) * static_cast<size_t>(vertexY - 1) * 2 };

	const float halfWidth{ 0.5f * width };
	const float halfDepth{ 0.5f * depth };

	const float dx{ width / (vertexY - 1) };	// x 기울기
	const float dz{ depth / (vertexX - 1) };	// z 기울기

	const float du{ 1.0f / (vertexY - 1) };
	const float dv{ 1.0f / (vertexX - 1) };

	std::vector<Vertex> vertices(vertexCount);
	std::vector<uint32_t> indices(faceCount * 3);

	for (size_t i = 0; i < vertexX; ++i) {
		float z{ halfDepth - i * dz };
		for (size_t j = 0; j < vertexY; ++j) {
			float x{ -halfWidth + j * dx };

			Vertex& v{ vertices[i * static_cast<size_t>(vertexY) + j] };

			v.pos = { x, 0.0f, z };
			v.normal = { 0.0f, 1.0f, 0.0f };
			v.texCoord.x = j * du;
			v.texCoord.y = i * dv;
		}
	}

	size_t k{ 0 };
	for (uint32_t i = 0; i < vertexX - 1; ++i) {
		for (uint32_t j = 0; j < vertexY - 1; ++j) {
			indices[k] = i * vertexY + j;
			indices[k + 1] = i * vertexY + j + 1;
			indices[k + 2] = (i + 1) * vertexY + j;

			indices[k + 3] = (i + 1) * vertexY + j;
			indices[k + 4] = i * vertexY + j + 1;
			indices[k + 5] = (i + 1) * vertexY + j + 1;

			k += 6;
		}
	}

	const UINT vbSize{ static_cast<UINT>(vertices.size()) * sizeof(Vertex) };
	const UINT ibSize{ static_cast<UINT>(indices.size()) * sizeof(uint32_t) };

	vBuffer = Buffers::CreateDefaultBuffer(device, commandList,
		vertices.data(), vbSize, vUploadBuffer);
	iBuffer = Buffers::CreateDefaultBuffer(device, commandList,
		indices.data(), ibSize, iUploadBuffer);

	vByteSize = vbSize;
	vByteStride = sizeof(Vertex);

	iFormat = DXGI_FORMAT_R32_UINT;
	iByteSize = ibSize;

	iCount = static_cast<UINT>(indices.size());
}

GridXZ::~GridXZ()
{
}
