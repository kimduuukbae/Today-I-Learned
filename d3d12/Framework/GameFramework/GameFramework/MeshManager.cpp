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
	meshs["GridXZ"] = std::make_unique<GridXZ>(100, 100, 4112.0f, 4112.0f,
		device, cmdList);
}

MeshBase* MeshManager::GetMesh(const std::string& name)
{
	auto it = meshs.find(name);

	return it != meshs.end() ? it->second.get() : nullptr;
}

MeshBase* MeshManager::GetMeshFromFile(const std::string& path)
{
	return nullptr;
}

MeshBase* MeshManager::CreateMesh(std::vector<Vertex>* v, std::vector<uint32_t>* i, D3D_PRIMITIVE_TOPOLOGY pTopology, const std::string& name)
{
	if (meshs.find(name) != meshs.end())
		return nullptr;

	if (i != nullptr) {
		meshs[name] = std::make_unique<CustomIndexMesh>();
		auto p = static_cast<CustomIndexMesh*>(meshs[name].get());
		p->CreateMesh(v, i, pTopology, name);
	}
	else {
		meshs[name] = std::make_unique<CustomVertexMesh>();
		auto p = static_cast<CustomVertexMesh*>(meshs[name].get());
		p->CreateMesh(v, pTopology, name);
	}

	return meshs[name].get();
}

MeshBase* MeshManager::CreateFrameMesh(std::vector<struct Vertex>* v, std::vector<std::vector<uint32_t>>* indexCluster, D3D_PRIMITIVE_TOPOLOGY pTopology, const std::string& name)
{
	meshs[name] = std::make_unique<FrameMesh>();
	auto p = static_cast<FrameMesh*>(meshs[name].get());

	p->SetVertex(*v);
	for (auto& it : *indexCluster)
		p->AddSubMesh(it);

	return p;
}

void MeshManager::ReleaseUploadBuffer()
{
	for (auto& it : meshs)
		static_cast<MeshBase*>(it.second.get())->ReleaseUploadBuffer();
}
