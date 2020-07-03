#include "stdafx.h"
#include "GameObject.h"
#include "Shader.h"
#include "Camera.h"

CGameObject::CGameObject() {
	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());	// 단위행렬
}

CGameObject::~CGameObject() {
	if (mesh) mesh->Release();
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
	if (inMesh)
		mesh = inMesh;
}
void CGameObject::Animate(float elapsedTime) {

}

void CGameObject::OnPrepareRender(){

}

void CGameObject::Render(ID3D12GraphicsCommandList* commandList, CCamera* pCamera){
	OnPrepareRender();
	if (shader) {
		shader->UpdateShaderVariables(commandList, &worldMatrix);
		shader->Render(commandList, pCamera);
	}
	if (mesh) mesh->Render(commandList);
}

void CGameObject::Rotate(XMFLOAT3* pxmf3Axis, float fAngle){
	XMMATRIX rotate{ XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle)) };
	worldMatrix = Matrix4x4::Multiply(rotate, worldMatrix);
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
