#pragma once

#include "Frame.h"

class Player : public Object
{
public:
	Player();
	virtual ~Player() = default;

	virtual void Init() override;
	virtual void Draw(ID3D12GraphicsCommandList* cmdList) override;

	void LoadFrameHierarchyFromFile();
	void LoadMaterials(FILE* pInFile);

	void LeftKey();
	void RightKey();
	void UpKey();
	void DownKey();

	void PitchUp();
	void PitchDown();
	void RotateLeft();
	void RotateRight();
private:
	std::unique_ptr<class CameraComponent> cameraComponent;
	class InputComponent* inputComponent;

	std::unique_ptr<Frame> frame;
};

