#include "stdafx.h"
#include "EnemyBox.h"
#include "Player.h"
using namespace DirectX;
void CPlayer::SetPosition(float x, float y, float z){
	position = XMFLOAT3{ x,y,z };
	CGameObject::SetPosition(x, y, z);
}

void CPlayer::SetRotation(float x, float y, float z){

	if (m_pCamera)
		m_pCamera->SetRotation(x, y, z);
}

void CPlayer::LookAt(XMFLOAT3& LookAt, XMFLOAT3& up) {
	XMFLOAT4X4 tView{};
	XMStoreFloat4x4(&tView,
	XMMatrixLookAtLH(XMLoadFloat3(&position),
	XMLoadFloat3(&LookAt), XMLoadFloat3(&up)));

	XMVECTORF32 tRight{ tView._11, tView._21, tView._31, 0.0f };
	XMVECTORF32 tUp{ tView._12, tView._22, tView._32, 0.0f };
	XMVECTORF32 tLook{ tView._13, tView._23, tView._33, 0.0f };

	XMStoreFloat3(&right, XMVector3Normalize(tRight));
	XMStoreFloat3(&this->up, XMVector3Normalize(tUp));
	XMStoreFloat3(&look, XMVector3Normalize(tLook));
}

void CPlayer::Update(float fElapsedTime) {
	Move(velocity, false);

	m_pCamera->Update(this, position, fElapsedTime);
	m_pCamera->GenerateViewMatrix();

	XMVECTOR tVelocity{ XMLoadFloat3(&velocity) };
	XMVECTOR decel{ XMVector3Normalize(XMVectorScale(tVelocity, -1.0f)) };

	float fLength{ XMVectorGetX(XMVector3Length(tVelocity)) };
	float fDecel{ m_fFriction * fElapsedTime };

	if (fDecel > fLength) fDecel = fLength;
	XMStoreFloat3(&velocity, XMVectorAdd(tVelocity,
	XMVectorScale(decel, fDecel)));
}

void CPlayer::OnUpdateTransform(){
	worldMatrix._11 = right.x;
	worldMatrix._12 = right.y;
	worldMatrix._13 = right.z;
	worldMatrix._21 = up.x;
	worldMatrix._22 = up.y;
	worldMatrix._23 = up.z;
	worldMatrix._31 = look.x;
	worldMatrix._32 = look.y;
	worldMatrix._33 = look.z;
	worldMatrix._41 = position.x;
	worldMatrix._42 = position.y;
	worldMatrix._43 = position.z;
}

void CPlayer::Animate(float fElapsedTime){
	OnUpdateTransform();
	CGameObject::Animate(fElapsedTime);
}

void CPlayer::Move(DWORD dwDirection, float fDistance){
	if (dwDirection) {
		XMFLOAT3 shift{ 0.0f, 0.0f, 0.0f };
		if (dwDirection & DIR_FORWARD)
			XMStoreFloat3(&shift, XMVectorAdd(XMLoadFloat3(&shift),
				XMVectorScale(XMLoadFloat3(&look), fDistance)));
		if(dwDirection & DIR_BACKWARD)
			XMStoreFloat3(&shift, XMVectorAdd(XMLoadFloat3(&shift),
				XMVectorScale(XMLoadFloat3(&look), -fDistance)));
		if (dwDirection & DIR_RIGHT)
			XMStoreFloat3(&shift, XMVectorAdd(XMLoadFloat3(&shift),
				XMVectorScale(XMLoadFloat3(&right), fDistance)));
		if (dwDirection & DIR_LEFT)
			XMStoreFloat3(&shift, XMVectorAdd(XMLoadFloat3(&shift),
				XMVectorScale(XMLoadFloat3(&right), -fDistance)));
		if (dwDirection & DIR_UP)
			XMStoreFloat3(&shift, XMVectorAdd(XMLoadFloat3(&shift),
				XMVectorScale(XMLoadFloat3(&up), fDistance)));
		if (dwDirection & DIR_DOWN)
			XMStoreFloat3(&shift, XMVectorAdd(XMLoadFloat3(&shift),
				XMVectorScale(XMLoadFloat3(&up), -fDistance)));
		Move(shift, true);
	}
}

void CPlayer::Move(XMFLOAT3& shift, bool updateVelocity){
	if (updateVelocity) {
		XMStoreFloat3(&velocity,
			XMVectorAdd(XMLoadFloat3(&velocity), XMLoadFloat3(&shift)));
	}
	else {
		XMStoreFloat3(&position,
			XMVectorAdd(XMLoadFloat3(&position), XMLoadFloat3(&shift)));
		if (m_pCamera) m_pCamera->Move(shift);
	}
}

void CPlayer::Move(float x, float y, float z){
	XMFLOAT3 tMove{ x,y,z };
	Move(tMove, false);
}

void CPlayer::Rotate(float fPitch, float fYaw, float fRoll){

		m_pCamera->Rotate(fPitch, fYaw, fRoll);
	// 플레이어를 로컬 x, y, z 축 중심으로 회전
	if (fPitch != 0.0f) {
		XMMATRIX tRotate{ XMMatrixRotationAxis(XMLoadFloat3(&right),
			XMConvertToRadians(fPitch)) };
		XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), tRotate));
		XMStoreFloat3(&up, XMVector3TransformNormal(XMLoadFloat3(&up), tRotate));
	}
	if (fYaw != 0.0f) {
		XMMATRIX tRotate{ XMMatrixRotationAxis(XMLoadFloat3(&up),
			XMConvertToRadians(fYaw)) };
		XMStoreFloat3(&look, XMVector3TransformNormal(XMLoadFloat3(&look), tRotate));
		XMStoreFloat3(&right, XMVector3TransformNormal(XMLoadFloat3(&right), tRotate));
	}
	if (fRoll != 0.0f) {
		XMMATRIX tRotate{ XMMatrixRotationAxis(XMLoadFloat3(&look),
			XMConvertToRadians(fRoll)) };
		XMStoreFloat3(&up, XMVector3TransformNormal(XMLoadFloat3(&up), tRotate));
		XMStoreFloat3(&right, XMVector3TransformNormal(XMLoadFloat3(&right), tRotate));
	}
	// 회전한후 직교하지 않을수도 있으니 다시 직교하게 만듬
	XMVECTOR tLook{ XMVector3Normalize(XMLoadFloat3(&look)) };
	XMVECTOR tUp{ XMVector3Normalize(XMLoadFloat3(&up)) };
	XMVECTOR tRight{ XMVector3Normalize(XMVector3Cross(tUp, tLook)) };
	tUp = XMVector3Normalize(XMVector3Cross(tLook, tRight));

	XMStoreFloat3(&look, tLook);
	XMStoreFloat3(&right, tRight);
	XMStoreFloat3(&up, tUp);
}

void CPlayer::SetCameraOffset(const XMFLOAT3& cameraOffset){
	this->cameraOffset = cameraOffset;
	XMFLOAT3 cameraPos{};
	XMStoreFloat3(&cameraPos,
	XMVectorAdd(XMLoadFloat3(&position), XMLoadFloat3(&this->cameraOffset)));
	
	m_pCamera->SetLookAt(cameraPos, position, up);

	m_pCamera->GenerateViewMatrix();
}

void CAirplanePlayer::OnUpdateTransform(){
	CPlayer::OnUpdateTransform();

	XMStoreFloat4x4(&worldMatrix, XMMatrixMultiply
	(XMMatrixRotationRollPitchYaw(XMConvertToRadians(90.0f), 0.0f, 0.0f),
		XMLoadFloat4x4(&worldMatrix)));
}

void CAirplanePlayer::Update(float fElapsedTime){
	CPlayer::Update(fElapsedTime);

	for (auto it = bullets.begin(); it != bullets.end();) {
		if (!(*it)->GetLive() || XMVectorGetX(XMVector3LengthEst(
			XMVectorSubtract(XMLoadFloat3(&(*it)->GetPosition()), XMLoadFloat3(&position)))) > 500.0f) {
			delete (*it);
			it = bullets.erase(it);
		}
		else 
			++it;
	}
}

void CAirplanePlayer::ShootBullet(){
	CCubeMesh* pMesh{ new CCubeMesh{1.0f, 1.0f, 1.0f} };

	CBullet* bullet{ new CBullet };
	bullet->SetMesh(pMesh);
	bullet->SetColor(RGB(255, 0, 0));
	bullet->SetPosition(position);
	bullet->SetMovingDirection(look);
	bullet->SetMovingSpeed(100.0f);
	
	if(pickingTarget){
		if (CEnemyBox* enemy{ dynamic_cast<CEnemyBox*>(pickingTarget) }; 
			enemy && !enemy->GetTargeting()) {
			enemy->SetTargeting(true);
			bullet->SetTarget(pickingTarget);
			pickingTarget = nullptr;
			bullet->SetColor(RGB(0, 0, 255));
		}
	}
	bullets.push_back(bullet);
}

void CAirplanePlayer::SetPickingObject(CGameObject* object){
	pickingTarget = object;
}

CBullet::CBullet(){
	XMVECTOR minVect{ XMVectorSet(-0.5f, -0.5f, -0.5f, 1.0f) };
	XMVECTOR maxVect{ XMVectorSet(0.5f, 0.5f, 0.5f, 1.0f) };

	BoundingBox box{};
	BoundingBox::CreateFromPoints(box, minVect, maxVect);
}

BoundingBox CBullet::getCollisionBox(){
	BoundingBox curColl{ boundBox };
	curColl.Transform(curColl, XMLoadFloat4x4(&worldMatrix));
	return curColl;
}

void CBullet::SetLive(bool bFlags){
	bLive = bFlags;
}

void CBullet::Animate(float fElapsedTime){
	CGameObject::Animate(fElapsedTime);
	if (shootTarget) {
		fShootTime += fElapsedTime;
		if (fShootTime > 0.3f) {
			XMStoreFloat3(&direction, XMVector3Normalize(XMVectorSubtract(
				XMLoadFloat3(&shootTarget->GetPosition()), XMLoadFloat3(&GetPosition()))));
		}
	}
}

void CBullet::SetTarget(CGameObject* target){
	shootTarget = target;
}
