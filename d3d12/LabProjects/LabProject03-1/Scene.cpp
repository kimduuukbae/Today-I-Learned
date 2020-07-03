#include "stdafx.h"
#include "Scene.h"
#include "Shader.h"

CScene::CScene() {}

CScene::~CScene() {}

void CScene::BuildObjects(const ComPtr<ID3D12Device>& device, ID3D12GraphicsCommandList* commandList) {
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(device.Get());

	shaderCount = 1;
	shaders = new CShader * [shaderCount];
	
	CShader* pShader{ new CShader{} };
	pShader->CreateShader(device.Get(), m_pd3dGraphicsRootSignature.Get());
	pShader->BuildObjects(device.Get(), commandList);

	shaders[0] = pShader;
}

void CScene::ReleaseObjects() {
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();

	if (shaders) {
		for (int i = 0; i < shaderCount; ++i) {
			shaders[i]->ReleaseShaderVariables();
			shaders[i]->ReleaseObjects();
			shaders[i]->Release();
		}
		delete[] shaders;
	}
}

bool CScene::ProcessInput(){
	return false;
}

void CScene::AnimateObjects(float fTimeElapsed){
	for (int i = 0; i < shaderCount; ++i)
		shaders[i]->AnimateObjects(fTimeElapsed);
}

void CScene::Render(const ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList){
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature.Get());
	
	for (int i = 0; i < shaderCount; ++i)
		shaders[i]->Render(pd3dCommandList.Get());
	//pd3dCommandList->SetPipelineState(m_pd3dPipelineState.Get());
	//pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//pd3dCommandList->DrawInstanced(3, 1, 0, 0);
}

ID3D12RootSignature* CScene::CreateGraphicsRootSignature(ID3D12Device* device){
	ID3D12RootSignature* rootSignature{ nullptr };
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.NumParameters = 0;
	rootSignatureDesc.pParameters = nullptr;
	rootSignatureDesc.NumStaticSamplers = 0;
	rootSignatureDesc.pStaticSamplers = nullptr;
	rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* signatureBlob{ nullptr };
	ID3DBlob* errorBlob{ nullptr };
	D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);
	device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&rootSignature));

	if (signatureBlob) signatureBlob->Release();
	if (errorBlob) errorBlob->Release();

	return rootSignature;
}

ID3D12RootSignature* CScene::GetGraphicsRootSignature(){
	return m_pd3dGraphicsRootSignature.Get();
}

void CScene::ReleaseUploadBuffers(){
	if (shaders) {
		for (int i = 0; i < shaderCount; ++i) {
			if (shaders[i])
				shaders[i]->ReleaseUploadBuffers();
		}
	}
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	return false;
}
bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	return false;
}