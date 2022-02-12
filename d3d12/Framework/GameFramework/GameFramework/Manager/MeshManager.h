#pragma once

#include "Singleton.h"

class MeshBase;

class MeshManager : public Singleton<MeshManager>
{
public:
	MeshManager();
	virtual ~MeshManager();

	void Init();

	MeshBase* GetMesh(const std::string& name);
	MeshBase* GetMeshFromFile(const std::string& path);

	MeshBase* CreateMesh(std::vector<struct Vertex>* v, std::vector<uint32_t>* i,
		D3D_PRIMITIVE_TOPOLOGY pTopology, const std::string& name);
	MeshBase* CreateFrameMesh(std::vector<struct Vertex>* v,
		std::vector<std::vector<uint32_t>>* indexCluster,
		D3D_PRIMITIVE_TOPOLOGY pTopology, const std::string& name);
	void ReleaseUploadBuffer();
private:

	std::unordered_map<std::string, std::unique_ptr<MeshBase>> meshs;
};

