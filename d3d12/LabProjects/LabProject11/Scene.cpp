#include "stdafx.h"
#include "Scene.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"

CScene::CScene() {}

CScene::~CScene() {}

void CScene::BuildObjects(const ComPtr<ID3D12Device>& device, ID3D12GraphicsCommandList* commandList) {
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(device.Get());
	
	m_nShaders = 1;
	m_pShaders = new CObjectsShader[m_nShaders];
	m_pShaders[0].CreateShader(device.Get(), m_pd3dGraphicsRootSignature.Get());
	m_pShaders[0].BuildObjects(device.Get(), commandList);
}

void CScene::ReleaseObjects() {
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();
	if (m_pShaders) {
		for (int i = 0; i < m_nShaders; ++i) {
			m_pShaders[i].ReleaseShaderVariables();
			m_pShaders[i].ReleaseObjects();
		}
		delete[] m_pShaders;
	}
}

bool CScene::ProcessInput(UCHAR* pKeysBuffer){
	return false;
}
bool CScene::ProcessInput() {
	return false;
}

void CScene::AnimateObjects(float fTimeElapsed){
	for (int i = 0; i < m_nShaders; ++i)
		m_pShaders[i].AnimateObjects(fTimeElapsed);
}

void CScene::Render(const ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, CCamera* pCamera){
	pCamera->SetViewportsAndScissorRects(pd3dCommandList.Get());
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature.Get());
	
	if (pCamera) pCamera->UpdateShaderVariables(pd3dCommandList.Get());
	for (int i = 0; i < m_nShaders; ++i)
		m_pShaders[i].Render(pd3dCommandList.Get(), pCamera);

	//pd3dCommandList->SetPipelineState(m_pd3dPipelineState.Get());
	//pd3dCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//pd3dCommandList->DrawInstanced(3, 1, 0, 0);
}

ID3D12RootSignature* CScene::CreateGraphicsRootSignature(ID3D12Device* device){
	ID3D12RootSignature* rootSignature{ nullptr };

	D3D12_ROOT_PARAMETER param[2];
	param[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	param[0].Constants.Num32BitValues = 16;	// world
	param[0].Constants.ShaderRegister = 0;
	param[0].Constants.RegisterSpace = 0;
	param[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	param[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	param[1].Constants.Num32BitValues = 32;	// camera, projection
	param[1].Constants.ShaderRegister = 1;
	param[1].Constants.RegisterSpace = 0;
	param[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_ROOT_SIGNATURE_FLAGS flags{ D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
	D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
	D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS | 
	D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS | 
	D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS };


	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc{};
	rootSignatureDesc.NumParameters = _countof(param);
	rootSignatureDesc.pParameters = param;
	rootSignatureDesc.NumStaticSamplers = 0;
	rootSignatureDesc.pStaticSamplers = nullptr;
	rootSignatureDesc.Flags = flags;

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
	for (int i = 0; i < m_nShaders; ++i)
		m_pShaders[i].ReleaseUploadBuffers();
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	return false;
}
bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	return false;
}