#pragma once

#include "Object.h"

class Billboard: public Object
{
public:
	Billboard();
	virtual ~Billboard() = default;

	virtual void Init() override;
	virtual void Draw(ID3D12GraphicsCommandList* cmdList) override;

private:
	std::unique_ptr<class MeshComponent> mesh;
	std::unique_ptr<class TextureComponent> texture;
};

