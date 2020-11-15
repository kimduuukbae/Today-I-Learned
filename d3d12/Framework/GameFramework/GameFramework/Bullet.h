#pragma once
#include "Object.h"
class Bullet : public Object
{
public:
	Bullet();
	virtual ~Bullet() = default;

	virtual void Init() override;
	virtual void Draw(ID3D12GraphicsCommandList* cmdList) override;
	virtual void Update(const GameTimer& gt) override;

private:
	std::unique_ptr<class MeshComponent> mesh;
	std::unique_ptr<class TextureComponent> texture;
};

