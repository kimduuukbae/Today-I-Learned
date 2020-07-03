#include "stdafx.h"
#include "GameObject.h"
#include "Shader.h"

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
	if (inShader) shader->Release();

	shader = inShader;
	if (inShader) inShader->AddRef();
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

void CGameObject::Render(ID3D12GraphicsCommandList* commandList){
	OnPrepareRender();
	if (shader) shader->Render(commandList);
	if (mesh) mesh->Render(commandList);
}
