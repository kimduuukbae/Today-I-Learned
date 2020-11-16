#pragma once

#include "CameraComponent.h"
#include "UpdateComponent.h"

class LagCameraComponent : public CameraComponent, public UpdateComponent
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

