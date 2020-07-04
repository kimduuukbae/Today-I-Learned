#include "stdafx.h"
#include "Bullet.h"

void CBullet::Animate(float elapsedTime){
	this->MoveUp(200.0f * elapsedTime);
}
