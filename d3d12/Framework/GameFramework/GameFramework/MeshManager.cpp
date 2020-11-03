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
}

Mesh* MeshManager::GetMesh(const std::string& name)
{
	return meshs[name].get();
}

Mesh* MeshManager::GetMeshFromFile(const std::string& path)
{
	return nullptr;
}

void MeshManager::ReleaseUploadBuffer()
{
	for (auto& it : meshs)
		static_cast<MeshBase*>(it.second.get())->ReleaseUploadBuffer();
}
