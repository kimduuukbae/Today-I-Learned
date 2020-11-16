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

protected:
	using Super = Monster;
};

