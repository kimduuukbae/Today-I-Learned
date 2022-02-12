#pragma once

#include "Object/Object.h"

class SkySphere : public Object
{
public:
	SkySphere();

	void Init();
	void Draw(ID3D12GraphicsCommandList* cmdList);

private:
	class MeshComponent* mesh;
	class TextureComponent* texture;
};

