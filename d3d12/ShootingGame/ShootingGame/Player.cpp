#include "stdafx.h"
#include "Player.h"
#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"

CPlayer::CPlayer() {

}

CPlayer::~CPlayer() {
	ReleaseShaderVariables();
	if (m_pCamera) delete m_pCamera;
}

void CPlayer::CreateShaderVariables(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList) {
	CGameObject::CreateShaderVariables(pd3dDevice, pd3dCommandList);
	if (m_pCamera) m_pCamera->CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CPlayer::ReleaseShaderVariables() {
	CGameObject::ReleaseShaderVariables();
	if (m_pCamera) m_pCamera->ReleaseShaderVariables();
}

void CPlayer::UpdateShaderVariables(ID3D12GraphicsCommandList* pd3dCommandList) {
	CGameObject::UpdateShaderVariables(pd3dCommandList);
}

void CPlayer::Move(ULONG dwDirection, float fDistance, bool bUpdateVelocity) {
	if (dwDirection){
		XMFLOAT3 xmf3Shift{ XMFLOAT3(0, 0, 0) };
		if (dwDirection & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, fDistance);
		if (dwDirection & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -fDistance);
		if (dwDirection & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, fDistance);
		if (dwDirection & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -fDistance);
		if (dwDirection & DIR_UP) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, fDistance);
		if (dwDirection & DIR_DOWN) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, -fDistance);
		Move(xmf3Shift, bUpdateVelocity);
	}
}

void CPlayer::Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity) {
	if (bUpdateVelocity)
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	else{
		m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);
		if (m_pCamera) m_pCamera->Move(xmf3Shift);
	}

	if (m_xmf3Position.x > 200.0f || m_xmf3Position.x < -200.0f ||
		m_xmf3Position.y > 200.0f || m_xmf3Position.y < -200.0f ||
		m_xmf3Position.z > 300.0f || m_xmf3Position.z < -300.0f)
		m_xmf3Position = XMFLOAT3{ 0.0f, 0.0f, 0.0f };
}

void CPlayer::Rotate(float x, float y, float z) {
	if (x != 0.0f){
		XMMATRIX xmmtxRotate{ XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(x)) };
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
	}
	if (y != 0.0f){
		XMMATRIX xmmtxRotate{ XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y)) };
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
	}
	if (z != 0.0f){
		XMMATRIX xmmtxRotate{ XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look), XMConvertToRadians(z)) };
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
	}
	m_pCamera->Rotate(x, y, z);
	if (y != 0.0f){
		XMMATRIX xmmtxRotate{ XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y)) };
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
	}

	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
}


void CPlayer::Update(float fTimeElapsed) {


	XMFLOAT3 xmf3Velocity{ Vector3::ScalarProduct(m_xmf3Velocity, fTimeElapsed, false) };
	Move(xmf3Velocity, false);

	if (m_pPlayerUpdatedContext) OnPlayerUpdateCallback(fTimeElapsed);

	m_pCamera->Update(m_xmf3Position, fTimeElapsed);

	if (m_pCameraUpdatedContext) OnCameraUpdateCallback(fTimeElapsed);

	m_pCamera->SetLookAt(m_xmf3Position);

	m_pCamera->RegenerateViewMatrix();

	float fDeceleration{ m_fFriction * fTimeElapsed };
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));
}

void CPlayer::OnPrepareRender() {
	worldMatrix._11 = m_xmf3Right.x;
	worldMatrix._12 = m_xmf3Right.y;
	worldMatrix._13 = m_xmf3Right.z;
	worldMatrix._21 = m_xmf3Up.x;
	worldMatrix._22 = m_xmf3Up.y;
	worldMatrix._23 = m_xmf3Up.z;
	worldMatrix._31 = m_xmf3Look.x;
	worldMatrix._32 = m_xmf3Look.y;
	worldMatrix._33 = m_xmf3Look.z;
	worldMatrix._41 = m_xmf3Position.x;
	worldMatrix._42 = m_xmf3Position.y;
	worldMatrix._43 = m_xmf3Position.z;
}

void CPlayer::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) {
	if (shader) shader->Render(pd3dCommandList, pCamera);
	CGameObject::Render(pd3dCommandList, pCamera);
}


CAirplanePlayer::CAirplanePlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	ID3D12RootSignature* pd3dGraphicsRootSignature){
	//비행기 메쉬를 생성한다. 
	CMesh* pAirplaneMesh{ new CAirplaneMeshDiffused(pd3dDevice, pd3dCommandList, 20.0f,
	20.0f, 4.0f, XMFLOAT4(0.0f, 0.5f, 0.0f, 0.0f)) };
	SetMesh(pAirplaneMesh);

	m_pCamera = new CThirdPersonCamera(m_pCamera);
	m_pCamera->SetPlayer(this);

	SetFriction(125.0f);
	SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
	SetMaxVelocityXZ(125.0f);
	SetMaxVelocityY(400.0f);

	m_pCamera->SetTimeLag(0.25f);
	m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, -50.0f));
	m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
	m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
	m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	SetPosition(XMFLOAT3(0.0f, 0.0f, -50.0f));

	CPlayerShader* pShader{ new CPlayerShader{} };
	pShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	SetShader(pShader);

	bulletShader = new CBulletShader{};
	bulletShader->CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
	bulletShader->BuildObjects(pd3dDevice, pd3dCommandList);
}

CAirplanePlayer::~CAirplanePlayer() {
	if (bulletShader) {
		bulletShader->ReleaseShaderVariables();
		bulletShader->Release();
	}
}

void CAirplanePlayer::OnPrepareRender(){
	CPlayer::OnPrepareRender();

	XMMATRIX mtxRotate{ XMMatrixRotationRollPitchYaw(XMConvertToRadians(90.0f), 0.0f, 0.0f) };
	worldMatrix = Matrix4x4::Multiply(mtxRotate, worldMatrix);
}

void CAirplanePlayer::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera* pCamera) {
	if (bulletShader) bulletShader->Render(pd3dCommandList, pCamera);
	CPlayer::Render(pd3dCommandList, pCamera);
}

void CAirplanePlayer::Update(float fTimeElapsed) {
	if (bulletShader) bulletShader->AnimateObjects(fTimeElapsed);
	CPlayer::Update(fTimeElapsed);
}

void CAirplanePlayer::ShootBullet() {
	bulletShader->addBullet(GetPosition(), worldMatrix, pickingTarget);
	pickingTarget = nullptr;
}
