#pragma once

#include "Frame.h"

class Monster : public Object
{
public:
	Monster();
	virtual ~Monster() = default;

	virtual void Init() override;
	virtual void Draw(ID3D12GraphicsCommandList * cmdList) override;
	virtual void Update(const GameTimer & gt) override;

	virtual void LoadFrameHierarchyFromFile();
	void LoadMaterials(FILE * pInFile);

protected:
	using Super = Object;

	std::unique_ptr<Frame> frame;
	Frame* mainRotor{ nullptr };
	Frame* tailRotor{ nullptr };

	Object* target{ nullptr };


	int hp{ 100 };
};