#pragma once

#include "Object.h"

class Terrain : public Object
{
public:
	Terrain();
	virtual ~Terrain() = default;

	virtual void Init() override;
	virtual void Draw(ID3D12GraphicsCommandList * cmdList) override;

private:
	std::unique_ptr<class MeshComponent> mesh;
	std::unique_ptr<class TextureComponent> texture;
};

