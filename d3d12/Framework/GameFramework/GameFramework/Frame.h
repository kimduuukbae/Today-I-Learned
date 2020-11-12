#pragma once

#include "Object.h"

class Frame : public Object
{
public:
	Frame();
	virtual ~Frame();

	virtual void Init() override;
	virtual void Draw(ID3D12GraphicsCommandList* cmdList) override;

	void AddChild(std::unique_ptr<Frame>&& child);
	void LoadMeshFromFile(FILE* pInFile);
	void LoadMaterialsFromFile(FILE* pInFile);
	static std::unique_ptr<Frame> LoadFrameHierarchyFromFile(FILE* pInFile);

protected:
	Frame* childPtr{ nullptr };

private:
	std::unique_ptr<class MeshComponent> mesh;
	std::unique_ptr<class TextureComponent> texture;
	std::vector<std::unique_ptr<Frame>> childs;
};