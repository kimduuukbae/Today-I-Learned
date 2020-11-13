#pragma once

#include "Object.h"

class Frame : public Object
{
public:
	Frame();
	virtual ~Frame();

	virtual void Init() override;
	virtual void Draw(ID3D12GraphicsCommandList* cmdList) override;
	virtual void Update(const GameTimer& gt) override;

	void AddChild(std::unique_ptr<Frame>&& child);
	void LoadMeshFromFile(FILE* pInFile);
	void LoadMaterialsFromFile(FILE* pInFile);
	void UpdateMeshMatrix(DirectX::XMFLOAT4X4* mat);
	Frame* FindFrame(const std::string_view& frameName);

	static std::unique_ptr<Frame> LoadFrameHierarchyFromFile(FILE* pInFile);

	DirectX::XMFLOAT4X4& GetLocalTransform();

private:
	std::unique_ptr<class MeshComponent> mesh;
	std::unique_ptr<class TextureComponent> texture;
	std::vector<std::unique_ptr<Frame>> childs;

	DirectX::XMFLOAT4X4 trsf;

	std::string frameName;
};