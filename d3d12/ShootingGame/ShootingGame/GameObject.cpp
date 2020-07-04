#include "stdafx.h"
#include "GameObject.h"
#include "Shader.h"
#include "Camera.h"

CGameObject::CGameObject() {
	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());	// 단위행렬
}

CGameObject::~CGameObject() {
	if (mesh) 
		mesh->Release();
	if (shader) {
		shader->ReleaseShaderVariables();
		shader->Release();
	}
}

void CGameObject::SetShader(CShader* inShader){
	shader = inShader;
}

void CGameObject::ReleaseUploadBuffers(){
	if (mesh) mesh->ReleaseUploadBuffers();
}

void CGameObject::SetMesh(CMesh* inMesh) {
	if (inMesh) {
		mesh = inMesh;
		mesh->AddRef();
	}
		
}
void CGameObject::Animate(float elapsedTime) {

}

void CGameObject::OnPrepareRender(){

}

void CGameObject::Render(ID3D12GraphicsCommandList* commandList, CCamera* pCamera){
	OnPrepareRender();
	UpdateShaderVariables(commandList);
	if (shader) {
		shader->UpdateShaderVariables(commandList, &worldMatrix);
		shader->Render(commandList, pCamera);
	}
	if (mesh) mesh->Render(commandList);
}

void CGameObject::Render(ID3D12GraphicsCommandList* commandList, CCamera* pCamera, UINT nInstances){
	OnPrepareRender();
	if (mesh) mesh->Render(commandList, nInstances);
}

void CGameObject::Rotate(XMFLOAT3* pxmf3Axis, float fAngle){
	XMMATRIX rotate{ XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle)) };
	worldMatrix = Matrix4x4::Multiply(rotate, worldMatrix);
}
void CGameObject::Rotate(float fPitch, float fYaw, float fRoll) {
	XMMATRIX rotate{ XMMatrixRotationRollPitchYaw(XMConvertToRadians(fPitch), XMConvertToRadians(fYaw), XMConvertToRadians(fRoll)) };
	worldMatrix = Matrix4x4::Multiply(rotate, worldMatrix);
}
void CGameObject::CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList){
}

void CGameObject::UpdateShaderVariables(ID3D12GraphicsCommandList* commandList){
	XMFLOAT4X4 world{};
	XMStoreFloat4x4(&world, XMMatrixTranspose(XMLoadFloat4x4(&worldMatrix)));
	commandList->SetGraphicsRoot32BitConstants(0, 16, &world, 0);
}

void CGameObject::ReleaseShaderVariables(){
}

void CGameObject::SetPosition(float x, float y, float z) {
	worldMatrix._41 = x;
	worldMatrix._42 = y;
	worldMatrix._43 = z;
}

XMFLOAT3 CGameObject::GetPosition() {
	return XMFLOAT3{ worldMatrix._41, worldMatrix._42, worldMatrix._43 };
}

XMFLOAT3 CGameObject::GetLook(){
	return Vector3::Normalize(XMFLOAT3{ worldMatrix._31, worldMatrix._32, worldMatrix._33 });
}

XMFLOAT3 CGameObject::GetUp(){
	return Vector3::Normalize(XMFLOAT3{ worldMatrix._21, worldMatrix._22, worldMatrix._23 });
}

XMFLOAT3 CGameObject::GetRight(){
	return Vector3::Normalize(XMFLOAT3{ worldMatrix._11, worldMatrix._12, worldMatrix._13 });
}

void CGameObject::SetPosition(const XMFLOAT3& pos) {
	SetPosition(pos.x, pos.y, pos.z);
}

void CGameObject::MoveStrafe(float fDistance){
	SetPosition(Vector3::Add(GetPosition(), GetRight(), fDistance));
}

void CGameObject::MoveUp(float fDistance){
	SetPosition(Vector3::Add(GetPosition(), GetUp(), fDistance));
}

void CGameObject::MoveForward(float fDistance){
	SetPosition(Vector3::Add(GetPosition(), GetLook(), fDistance));
}

CRotatingObject::CRotatingObject(){
	rotatingAxis = XMFLOAT3{ 0.0f, 1.0f, 0.0f };
	rotationSpeed = 90.0f;
}

CRotatingObject::~CRotatingObject(){
}

void CRotatingObject::Animate(float elapsedTime){
	CGameObject::Rotate(&rotatingAxis, rotationSpeed * elapsedTime);
}

