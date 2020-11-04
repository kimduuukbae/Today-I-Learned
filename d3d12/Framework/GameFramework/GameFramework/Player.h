#pragma once

#include "Object.h"

class Player : public Object
{
public:
	Player();
	virtual ~Player() = default;

	virtual void Init() override;
	virtual void Draw(ID3D12GraphicsCommandList* cmdList) override;


	void LeftKey();
	void RightKey();
	void UpKey();
	void DownKey();
private:
	std::unique_ptr<class CameraComponent> cameraComponent;
	class InputComponent* inputComponent;
};

