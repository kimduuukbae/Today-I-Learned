#include "stdafx.h"
#include "Player.h"

void CPlayer::SetPosition(float x, float y, float z){
	CGameObject::SetPosition(x, y, z);
	if (m_pCamera)
		m_pCamera->SetPosition(x, y, z);
}

void CPlayer::SetRotation(float x, float y, float z){
	CGameObject::SetRotation(x, y, z);
	if (m_pCamera)
		m_pCamera->SetRotation(x, y, z);
}

void CPlayer::Move(float x, float y, float z){
	if (m_pCamera)
		m_pCamera->Move(x, y, z);
	CGameObject::Move(x, y, z);
}

void CPlayer::Rotate(float fPitch, float fYaw, float fRoll){
	if (m_pCamera)
		m_pCamera->Rotate(fPitch, fYaw, fRoll);
	CGameObject::Rotate(fPitch, fYaw, fRoll);
}
