#pragma once
#include "GameObject.h"

class CEnemyBox : public CGameObject{
private:
	std::vector<CGameObject*> particles{};
	BoundingBox boundBox{};
	bool bLive{ true };
	float fDieTime{ 0.0f };
public:
	CEnemyBox();
	virtual ~CEnemyBox();

	void SetParticles(CCubeMesh* mesh, DWORD dwColor, int particleCount = 25);
	
	void SetLive(bool bFlags);
	constexpr bool GetLive() {
		return bLive;
	}
	virtual void Animate(float fElapsedTime) override;
	virtual void Render(HDC hDCFrameBuffer, CCamera* pCamera) override;

	BoundingBox getCollisionBox();
};

