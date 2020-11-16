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

protected:
	using Super = Monster;
};

