#include "stdafx.h"
#include "BasicMesh.h"

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
}

SphereMesh::~SphereMesh(){}