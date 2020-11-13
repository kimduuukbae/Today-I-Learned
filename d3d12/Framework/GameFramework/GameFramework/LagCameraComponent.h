#pragma once

#include "CameraComponent.h"
#include "IComponent.h"

class LagCameraComponent : public CameraComponent, public IComponent
{
public:
	LagCameraComponent();
	virtual ~LagCameraComponent();

	virtual void Update(const GameTimer& gt) override;
	void SetLagScale(float scale);

	void SetLookAt(const DirectX::XMFLOAT3& lookAt);
private:

	float lagScale{ 0.25f };
};

