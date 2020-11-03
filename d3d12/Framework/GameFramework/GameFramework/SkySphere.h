#pragma once

#include "Object.h"

class SkySphere : public Object
{
public:
	SkySphere();

	void Init();
	void Draw(ID3D12GraphicsCommandList* cmdList);

private:
	std::unique_ptr<class MeshComponent> mesh;
	std::unique_ptr<class TextureComponent> texture;
};

