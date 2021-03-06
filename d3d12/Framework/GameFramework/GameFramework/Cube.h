#pragma once

#include "Object.h"

class Cube : public Object
{
public:
	Cube();
	virtual ~Cube() = default;

	virtual void Init() override;
	virtual void Draw(ID3D12GraphicsCommandList* cmdList) override;

private:
	class MeshComponent* mesh;
	class TextureComponent* texture;
};

