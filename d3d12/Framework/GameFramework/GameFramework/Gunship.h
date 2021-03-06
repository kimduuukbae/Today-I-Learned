#pragma once
#include "Monster.h"

class Gunship : public Monster
{
public:
	Gunship();
	virtual ~Gunship() = default;

	virtual void Init() override;
	virtual void Draw(ID3D12GraphicsCommandList* cmdList) override;
	virtual void Update(const GameTimer& gt) override;

	virtual void LoadFrameHierarchyFromFile();

	void ProcessCollision(class CollisionComponent& other);
protected:
	using Super = Monster;

	CollisionComponent* collision;
};

