#pragma once

#include "Object.h"

class Particle : public Object
{
public:
	Particle();
	virtual ~Particle();

	virtual void Init() override;
	virtual void Draw(ID3D12GraphicsCommandList* cmdList) override;
	virtual void Update(const GameTimer& gt) override;
private:
	std::unique_ptr<class MeshComponent> mesh;
	std::unique_ptr<class TextureComponent> texture;

	float xSize;
};

