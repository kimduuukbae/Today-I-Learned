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

	void ProcessCollision(class CollisionComponent& other);

private:
	std::unique_ptr<class MeshComponent> mesh;
	std::unique_ptr<class TextureComponent> texture;
	std::unique_ptr<class CollisionComponent> collision;

	DirectX::XMFLOAT3 originPos;
};

