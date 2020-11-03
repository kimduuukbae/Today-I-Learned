#pragma once

#include "Singleton.h"

class Mesh;

class MeshManager : public Singleton<MeshManager>
{
public:
	MeshManager();
	virtual ~MeshManager();

	void Init();

	Mesh* GetMesh(const std::string& name);
	Mesh* GetMeshFromFile(const std::string& path);

	void ReleaseUploadBuffer();
private:

	std::unordered_map<std::string, std::unique_ptr<Mesh>> meshs;
};

