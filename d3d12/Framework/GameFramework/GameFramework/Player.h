#pragma once

#include "Frame.h"

class Player : public Object
{
public:
	Player();
	virtual ~Player() = default;

	virtual void Init() override;
	virtual void Draw(ID3D12GraphicsCommandList* cmdList) override;
	virtual void Update(const GameTimer& gt) override;
	
	void LoadFrameHierarchyFromFile();
	void LoadMaterials(FILE* pInFile);

	void LeftKey();
	void RightKey();
	void UpKey();
	void DownKey();
	void CtrlKey();

	void MouseLeft(float f);
private:
	class LagCameraComponent* cameraComponent;
	class InputComponent* inputComponent;

	std::unique_ptr<Frame> frame;

	Frame* mainRotor{ nullptr };
	Frame* tailRotor{ nullptr };
};

