#include "stdafx.h"
#include "D3DApp.h"
#include "MeshManager.h"
#include "BasicMesh.h"
#include "Mesh.h"

MeshManager::MeshManager()
{

}

MeshManager::~MeshManager()
{
	meshs.clear();
}

void MeshManager::Init()
{
	auto device{ D3DApp::GetApp()->GetDevice() };
	auto cmdList{ D3DApp::GetApp()->GetCommandList() };
	meshs["Cube"] = std::make_unique<CubeMesh>(2.0f, 2.0f, 2.0f,
		device, cmdList);
	meshs["Sphere"] = std::make_unique<SphereMesh>(1.0f, 5, 5,
		device, cmdList);
	meshs["Landscape"] = std::make_unique<Landscape>(257, 257,
		device, cmdList);
}

Mesh* MeshManager::GetMesh(const std::string& name)
{
	return meshs[name].get();
}

Mesh* MeshManager::GetMeshFromFile(const std::string& path)
{
	return nullptr;
}

Mesh* MeshManager::CreateMesh(std::vector<Vertex>* v, std::vector<uint32_t>* i, D3D_PRIMITIVE_TOPOLOGY pTopology, const std::string& name)
{
	if (meshs.find(name) != meshs.end())
		return nullptr;

	meshs[name] = std::make_unique<CustomMesh>();
	auto p = static_cast<CustomMesh*>(meshs[name].get());
	p->CreateMesh(v, i, pTopology, name);
	
	return p;
}

void MeshManager::ReleaseUploadBuffer()
{
	for (auto& it : meshs)
		static_cast<MeshBase*>(it.second.get())->ReleaseUploadBuffer();
}
