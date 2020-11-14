#include "stdafx.h"
#include "Frame.h"
#include "MeshComponent.h"
#include "Mesh.h"
#include "MeshManager.h"
#include "ResourceManager.h"
#include "TextureComponent.h"
#include "Common.h"

Frame::Frame(){}

Frame::~Frame(){}

void Frame::Init()
{
	mesh = AddComponent<MeshComponent>();
	texture = AddComponent<TextureComponent>();
}

void Frame::Draw(ID3D12GraphicsCommandList* cmdList)
{
	if (mesh->IsBindingMesh()) {
		mesh->BindingResource(cmdList);
		texture->BindingResource(cmdList);
		cmdList->SetGraphicsRootConstantBufferView(0, GetTransform()->GetResourceAddress());
		mesh->Draw(cmdList);
	}

	for (auto& elem : childs)
		elem->Draw(cmdList);
}

void Frame::Update(const GameTimer& gt)
{
	Super::Update(gt);
}

void Frame::AddChild(std::unique_ptr<Frame>&& child)
{
	childs.emplace_back(std::move(child));
}

void Frame::LoadMeshFromFile(FILE* pInFile)
{
	using namespace DirectX;
	
	char pstrToken[64] = { '\0' };
	char m_pstrMeshName[64];
	BYTE nStrLength = 0;

	int nPositions = 0, nColors = 0, nNormals = 0, nTangents = 0, nBiTangents = 0, nTextureCoords = 0, nIndices = 0, nSubMeshes = 0, nSubIndices = 0;
	int m_nVertices;

	UINT nReads = (UINT)::fread(&m_nVertices, sizeof(int), 1, pInFile);
	nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
	nReads = (UINT)::fread(m_pstrMeshName, sizeof(char), nStrLength, pInFile);
	m_pstrMeshName[nStrLength] = '\0';

	std::vector<Vertex> v;
	std::vector<std::vector<uint32_t>> indexCluster;

	for (; ; )
	{
		nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
		nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
		pstrToken[nStrLength] = '\0';

		if (!strcmp(pstrToken, "<Bounds>:"))
		{
			DirectX::XMFLOAT3 m_xmf3AABBCenter, m_xmf3AABBExtents;
			nReads = (UINT)::fread(&m_xmf3AABBCenter, sizeof(XMFLOAT3), 1, pInFile);
			nReads = (UINT)::fread(&m_xmf3AABBExtents, sizeof(XMFLOAT3), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Positions>:"))
		{
			nReads = (UINT)::fread(&nPositions, sizeof(int), 1, pInFile);
			if (nPositions > 0)
			{
				XMFLOAT3* pos = new XMFLOAT3[nPositions];
				nReads = (UINT)::fread(pos, sizeof(XMFLOAT3), nPositions, pInFile);
				v.reserve(nPositions);

				for (int i = 0; i < nPositions; ++i)
					v.emplace_back(pos[i].x, pos[i].y, pos[i].z,
						0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
				delete[] pos;
			}
		}
		else if (!strcmp(pstrToken, "<Colors>:"))
		{
			nReads = (UINT)::fread(&nColors, sizeof(int), 1, pInFile);
			if (nColors > 0)
			{
				XMFLOAT4* colors  = new XMFLOAT4[nColors];
				nReads = (UINT)::fread(colors, sizeof(XMFLOAT4), nColors, pInFile);
				delete[] colors;
			}
		}
		else if (!strcmp(pstrToken, "<TextureCoords0>:"))
		{
			nReads = (UINT)::fread(&nTextureCoords, sizeof(int), 1, pInFile);
			if (nTextureCoords > 0)
			{
				XMFLOAT2* texCoord = new XMFLOAT2[nTextureCoords];
				nReads = (UINT)::fread(texCoord, sizeof(XMFLOAT2), nTextureCoords, pInFile);
					
				for (int i = 0; i < v.size(); ++i)
					v[i].texCoord = texCoord[i];

				delete[] texCoord;
			}
		}
		else if (!strcmp(pstrToken, "<TextureCoords1>:"))
		{
			nReads = (UINT)::fread(&nTextureCoords, sizeof(int), 1, pInFile);
			if (nTextureCoords > 0)
			{
				XMFLOAT2* texCoord = new XMFLOAT2[nTextureCoords];
				nReads = (UINT)::fread(texCoord, sizeof(XMFLOAT2), nTextureCoords, pInFile);
				
				delete[] texCoord;
			}
		}
		else if (!strcmp(pstrToken, "<Normals>:"))
		{
			nReads = (UINT)::fread(&nNormals, sizeof(int), 1, pInFile);

			if (nNormals > 0)
			{
				XMFLOAT3* normals = new XMFLOAT3[nNormals];
				nReads = (UINT)::fread(normals, sizeof(XMFLOAT3), nNormals, pInFile);

				for (int i = 0; i < v.size(); ++i)
					v[i].normal = normals[i];

				delete[] normals;
			}
		}
		else if (!strcmp(pstrToken, "<Tangents>:"))
		{
			nReads = (UINT)::fread(&nTangents, sizeof(int), 1, pInFile);
			if (nTangents > 0)
			{
				XMFLOAT3* m_pxmf3Tangents = new XMFLOAT3[nTangents];
				nReads = (UINT)::fread(m_pxmf3Tangents, sizeof(XMFLOAT3), nTangents, pInFile);
				
				delete[] m_pxmf3Tangents;
			}
		}
		else if (!strcmp(pstrToken, "<BiTangents>:"))
		{
			nReads = (UINT)::fread(&nBiTangents, sizeof(int), 1, pInFile);
			if (nBiTangents > 0)
			{
				XMFLOAT3* m_pxmf3BiTangents = new XMFLOAT3[nBiTangents];
				nReads = (UINT)::fread(m_pxmf3BiTangents, sizeof(XMFLOAT3), nBiTangents, pInFile);

				delete[] m_pxmf3BiTangents;
			}
		}
		else if (!strcmp(pstrToken, "<SubMeshes>:"))
		{
			int subMeshCount{};
			nReads = (UINT)::fread(&subMeshCount, sizeof(int), 1, pInFile);
			indexCluster.reserve(subMeshCount);
			if (subMeshCount > 0)
			{
				for (int i = 0; i < subMeshCount; i++)
				{
					int subIndexCount{};
					nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
					nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
					pstrToken[nStrLength] = '\0';
					if (!strcmp(pstrToken, "<SubMesh>:"))
					{
						int nIndex = 0;
						nReads = (UINT)::fread(&nIndex, sizeof(int), 1, pInFile);
						nReads = (UINT)::fread(&subIndexCount, sizeof(int), 1, pInFile);
						if (subIndexCount > 0)
						{
							indexCluster.emplace_back();
							indexCluster.back().resize(subIndexCount);
							nReads = (UINT)::fread(&indexCluster.back()[0], sizeof(UINT) * subIndexCount, 1, pInFile);
						}
					}
				}
			}
		}
		else if (!strcmp(pstrToken, "</Mesh>"))
		{
			break;
		}
	}
	mesh->SetMesh(MeshManager::GetInstance()->CreateFrameMesh(&v, &indexCluster, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		m_pstrMeshName));
}

void Frame::LoadMaterialsFromFile(FILE* pInFile)
{
	using namespace DirectX;
	char pstrToken[64] = { '\0' };

	int nMaterial = 0, m_nMaterials;
	BYTE nStrLength = 0;

	UINT nReads = (UINT)::fread(&m_nMaterials, sizeof(int), 1, pInFile);

	XMFLOAT4X4 temp;

	for (; ; )
	{
		nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
		nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
		pstrToken[nStrLength] = '\0';

		if (!strcmp(pstrToken, "<Material>:"))
		{
			nReads = (UINT)::fread(&nMaterial, sizeof(int), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<AlbedoColor>:"))
		{
			nReads = (UINT)::fread(&temp, sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<EmissiveColor>:"))
		{
			nReads = (UINT)::fread(&temp, sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<SpecularColor>:"))
		{
			nReads = (UINT)::fread(&temp, sizeof(float), 4, pInFile);
		}
		else if (!strcmp(pstrToken, "<Glossiness>:"))
		{
			nReads = (UINT)::fread(&temp, sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Smoothness>:"))
		{
			nReads = (UINT)::fread(&temp, sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<Metallic>:"))
		{
			nReads = (UINT)::fread(&temp, sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<SpecularHighlight>:"))
		{
			nReads = (UINT)::fread(&temp, sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<GlossyReflection>:"))
		{
			nReads = (UINT)::fread(&temp, sizeof(float), 1, pInFile);
		}
		else if (!strcmp(pstrToken, "<AlbedoMap>:"))
		{
			if (auto&& p = ResourceManager::GetInstance()->LoadTextureFromFile(pInFile); p)
				texture->SetTexture(p);
		}
		else if (!strcmp(pstrToken, "<SpecularMap>:"))
		{
			if (auto&& p = ResourceManager::GetInstance()->LoadTextureFromFile(pInFile); p)
				texture->AddTexture(p);
		}
		else if (!strcmp(pstrToken, "<NormalMap>:"))
		{
			if (auto&& p = ResourceManager::GetInstance()->LoadTextureFromFile(pInFile); p)
				texture->AddTexture(p);
		}
		else if (!strcmp(pstrToken, "<MetallicMap>:"))
		{
			if (auto&& p = ResourceManager::GetInstance()->LoadTextureFromFile(pInFile); p)
				texture->AddTexture(p);
		}
		else if (!strcmp(pstrToken, "<EmissionMap>:"))
		{
			if (auto&& p = ResourceManager::GetInstance()->LoadTextureFromFile(pInFile); p)
				texture->AddTexture(p);
		}
		else if (!strcmp(pstrToken, "<DetailAlbedoMap>:"))
		{
			if (auto&& p = ResourceManager::GetInstance()->LoadTextureFromFile(pInFile); p)
				texture->AddTexture(p);
		}
		else if (!strcmp(pstrToken, "<DetailNormalMap>:"))
		{
			if (auto&& p = ResourceManager::GetInstance()->LoadTextureFromFile(pInFile); p)
				texture->AddTexture(p);
		}
		else if (!strcmp(pstrToken, "</Materials>"))
		{
			break;
		}
	}
}

void Frame::UpdateMeshMatrix(DirectX::XMFLOAT4X4* mat)
{
	DirectX::XMFLOAT4X4& worldMat{ GetTransform()->GetTransformDirect() };
	worldMat = (mat) ? Math::Multiply(trsf, *mat) : trsf;

	GetTransform()->UpdateMatrix();

	for (auto& it : childs)
		it->UpdateMeshMatrix(&worldMat);
}

Frame* Frame::FindFrame(const std::string_view& frameName)
{
	if (this->frameName == frameName)
		return this;

	for (auto& it : childs)
		if (auto p = it->FindFrame(frameName); p)
			return p;
	
	return nullptr;
}


std::unique_ptr<Frame> Frame::LoadFrameHierarchyFromFile(FILE* pInFile)
{
	using namespace DirectX;

	char pstrToken[64] = { '\0' };

	BYTE nStrLength = 0;
	UINT nReads = 0;

	int nFrame = 0, nTextures = 0;

	std::unique_ptr<Frame> object{ nullptr };

	for (; ; )
	{
		nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
		nReads = (UINT)::fread(pstrToken, sizeof(char), nStrLength, pInFile);
		pstrToken[nStrLength] = '\0';

		if (!strcmp(pstrToken, "<Frame>:"))
		{
			char partsName[64];
			object = std::make_unique<Frame>();
			object->Init();

			nReads = (UINT)::fread(&nFrame, sizeof(int), 1, pInFile);
			nReads = (UINT)::fread(&nTextures, sizeof(int), 1, pInFile);

			nReads = (UINT)::fread(&nStrLength, sizeof(BYTE), 1, pInFile);
			nReads = (UINT)::fread(partsName, sizeof(char), nStrLength, pInFile);
			partsName[nStrLength] = '\0';
			object->frameName = partsName;
		}
		else if (!strcmp(pstrToken, "<Transform>:"))
		{
			XMFLOAT3 xmf3Position, xmf3Rotation, xmf3Scale;
			XMFLOAT4 xmf4Rotation;
			nReads = (UINT)::fread(&xmf3Position, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf3Rotation, sizeof(float), 3, pInFile); //Euler Angle
			nReads = (UINT)::fread(&xmf3Scale, sizeof(float), 3, pInFile);
			nReads = (UINT)::fread(&xmf4Rotation, sizeof(float), 4, pInFile); //Quaternion
		}
		else if (!strcmp(pstrToken, "<TransformMatrix>:"))
		{
			nReads = (UINT)::fread(&object->trsf, sizeof(float), 16, pInFile);
		}
		else if (!strcmp(pstrToken, "<Mesh>:"))
		{
			object->LoadMeshFromFile(pInFile);
		}
		else if (!strcmp(pstrToken, "<Materials>:"))
		{
			object->LoadMaterialsFromFile(pInFile);
		}
		else if (!strcmp(pstrToken, "<Children>:"))
		{
			int nChilds = 0;
			nReads = (UINT)::fread(&nChilds, sizeof(int), 1, pInFile);
			if (nChilds > 0)
			{
				for (int i = 0; i < nChilds; i++)
				{
					std::unique_ptr<Frame> pChild{ LoadFrameHierarchyFromFile(pInFile) };
					if (pChild) object->AddChild(std::move(pChild));
				}
			}
		}
		else if (!strcmp(pstrToken, "</Frame>"))
		{
			break;
		}
	}
	return std::move(object);
}

DirectX::XMFLOAT4X4& Frame::GetLocalTransform()
{
	return trsf;
}
