#pragma once

#include "Object.h"

class Water : public Object
{
public:
	Water();
	virtual ~Water() = default;

	virtual void Init() override;
	virtual void Draw(ID3D12GraphicsCommandList * cmdList) override;

private:
	class MeshComponent* mesh;
	class TextureComponent* texture;
};

