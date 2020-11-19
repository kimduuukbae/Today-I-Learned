#pragma once
#include "Monster.h"

class SuperCobra : public Monster
{
public:
	SuperCobra();
	virtual ~SuperCobra() = default;

	virtual void Init() override;
	virtual void Draw(ID3D12GraphicsCommandList* cmdList) override;
	virtual void Update(const GameTimer& gt) override;

	virtual void LoadFrameHierarchyFromFile();

	void ProcessCollision(class CollisionComponent& other);
protected:
	using Super = Monster;

	std::unique_ptr<class CollisionComponent> collision;
};

