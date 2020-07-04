#pragma once
#include "GameObject.h"

class CBullet : public CGameObject{
public:
	void Animate(float elapsedTime) override;
	void setDirection(XMFLOAT3& dir);

	XMFLOAT3 oldPos{};
protected:
	BoundingBox boundBox{};
	CGameObject* shootTarget{ nullptr };
};

