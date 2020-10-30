#pragma once

#include "Object.h"

class Player : public Object
{
public:
	Player();
	virtual ~Player() = default;

	virtual void Init() override;
	virtual void Draw(ID3D12GraphicsCommandList* cmdList) override;

private:
	std::unique_ptr<class CameraComponent> cameraComponent;
};

