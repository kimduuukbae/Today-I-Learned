#pragma once
#include "GameObject.h"

class CBullet : public CGameObject{
public:
	void Animate(float elapsedTime) override;
	void setDirection(XMFLOAT3& dir);

	void setTarget(CGameObject* target) {
		if (target)
			SetColor(XMFLOAT4{ 1.0f, 0.0f, 0.0f, 1.0f });
		
		shootTarget = target;
	}
	inline BoundingBox GetBoundingBox() {
		BoundingBox b{ boundBox };
		b.Transform(b, XMLoadFloat4x4(&worldMatrix));
		return b;
	}
	void SetLive(bool bFlag) {
		isLive = bFlag;
	}
	bool GetLive() {
		return isLive;
	}

	void SetColor(const XMFLOAT4& color) {
		diffuse = color;
	}
	XMFLOAT4& GetColor() {
		return diffuse;
	}
	XMFLOAT3 oldPos{};
protected:
	BoundingBox boundBox{};
	CGameObject* shootTarget{ nullptr };
	XMFLOAT3 direction{};
	XMFLOAT4 diffuse{};
	bool isLive{ true };
};

