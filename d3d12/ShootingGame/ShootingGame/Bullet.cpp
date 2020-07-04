#include "stdafx.h"
#include "Bullet.h"

void CBullet::Animate(float elapsedTime){
	if(!shootTarget)
		this->MoveUp(200.0f * elapsedTime);
	else {
		XMStoreFloat3(&direction, XMVector3Normalize(XMVectorSubtract(
			XMLoadFloat3(&shootTarget->GetPosition()), XMLoadFloat3(&GetPosition()))));
		SetPosition(Vector3::Add(GetPosition(), Vector3::ScalarProduct(direction, elapsedTime * 10.0f)));
	}
}
