#include "stdafx.h"
#include "Billboard.h"
#include "BasicMesh.h"
#include "TextureComponent.h"
#include "MeshComponent.h"
#include "MeshManager.h"
#include "GameplayStatics.h"
#include <fstream>
#include <functional>

Billboard::Billboard()
{
}

void Billboard::Init()
{
	mesh = AddComponent<MeshComponent>();
	texture = AddComponent<TextureComponent>();

	std::ifstream omIs{ "Textures\\ObjectsMap.raw", std::ios::binary };
	std::ifstream hmIs{ "Textures\\HeightMap.raw", std::ios::binary };
	std::vector<BYTE> heightMap(257 * 257);
	std::vector<BYTE> objectMap(257 * 257);

	hmIs.read(reinterpret_cast<char*>(&heightMap[0]), heightMap.size() - 1);
	omIs.read(reinterpret_cast<char*>(&objectMap[0]), objectMap.size() - 1);

	std::function<float(int, int)> GetHeight
	{
		[&heightMap](int x, int z) {
			return heightMap[static_cast<size_t>(x) + (static_cast<size_t>(z) * 257)] * 2.0f;
		}
	};

	std::function<unsigned char(int, int)> GetPixel
	{
		[&objectMap](int x, int z) {
			return objectMap[x + (z * 257)];
		}
	};

	int nGrassObjects = 0, nFlowerObjects = 0, nBlacks = 0, nOthers = 0, nTreeObjects[3] = { 0, 0, 0 };

	for (int z = 2; z <= 254; z++)
	{
		for (int x = 2; x <= 254; x++)
		{
			BYTE nPixel = GetPixel(x, z);
			switch (nPixel)
			{
			case 102: nGrassObjects++; break;
			case 128: nGrassObjects++; break;
			case 153: nFlowerObjects++; break;
			case 179: nFlowerObjects++; break;
			case 204: nTreeObjects[0]++; break;
			case 225: nTreeObjects[1]++; break;
			case 255: nTreeObjects[2]++; break;
			case 0: nBlacks++; break;
			default: nOthers++; break;
			}
		}
	}

	int nBillboardObjects = nGrassObjects + nFlowerObjects + nTreeObjects[0] + nTreeObjects[1] + nTreeObjects[2];

	std::vector<Vertex> objects;
	objects.reserve(nBillboardObjects);
	DirectX::XMFLOAT3 terrainScale{ 8.0f, 2.0f, 8.0f };


	for (int nObjects = 0, z = 2; z <= 254; z++)
	{
		for (int x = 2; x <= 254; x++)
		{
			BYTE nPixel = GetPixel(x, z);
			DirectX::XMFLOAT2 size;
			float fyOffset{};
			UINT nTexture{};
			switch (nPixel)
			{
			case 102:
				size = DirectX::XMFLOAT2(8.0f, 8.0f);
				fyOffset = 8.0f * 0.5f;
				nTexture = 0;
				break;
			case 128:
				size = DirectX::XMFLOAT2(8.0f, 8.0f);
				fyOffset = 6.0f * 0.5f;
				nTexture = 1;
				break;
			case 153:
				size = DirectX::XMFLOAT2(8.0f, 16.0f);
				fyOffset = 16.0f * 0.5f;
				nTexture = 2;
				break;
			case 179:
				size = DirectX::XMFLOAT2(8.0f, 16.0f);
				fyOffset = 16.0f * 0.5f;
				nTexture = 3;
				break;
			case 204:
				size = DirectX::XMFLOAT2(24.0f, 36.0f);
				fyOffset = 33.0f * 0.5f;
				nTexture = 4;
				break;
			case 225:
				size = DirectX::XMFLOAT2(24.0f, 36.0f);
				fyOffset = 33.0f * 0.5f;
				nTexture = 5;
				break;
			case 255:
				size = DirectX::XMFLOAT2(16.0f, 46.0f);
				fyOffset = 40.0f * 0.5f;
				nTexture = 6;
				break;
			default:
				break;
			}

			if (fyOffset != 0.0f)
			{
				float fHeight = GetHeight(x, z);
				float xPosition = x * terrainScale.x;
				float zPosition = z * terrainScale.z;

				Vertex v
				{
					xPosition, fHeight + fyOffset, zPosition,
					size.x, size.y, static_cast<float>(nTexture),
					0.0f, 0.0f
				};

				objects.push_back(v);
			}
		}
	}

	mesh->SetMesh(MeshManager::GetInstance()->CreateMesh(&objects, nullptr, D3D11_PRIMITIVE_TOPOLOGY_POINTLIST, "Billboard"));
	texture->SetTexture(GameplayStatics::GetTexture("Textures\\Flower01.dds"));
	texture->AddTexture(GameplayStatics::GetTexture("Textures\\Flower02.dds"));

	SetLayer(2);
}

void Billboard::Draw(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->SetPipelineState(GameplayStatics::GetPSO("Billboard"));

	mesh->BindingResource(cmdList);
	texture->BindingResource(cmdList);
	mesh->Draw(cmdList);
}
