#include "stdafx.h"
#include "Scene.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Shader.h"
#include "EnemyBox.h"
#include "Camera.h"
#include "Player.h"
#include "Bullet.h"

CScene::CScene() {}

CScene::~CScene() {}

void CScene::BuildObjects(const ComPtr<ID3D12Device>& device, ID3D12GraphicsCommandList* commandList) {
	m_pd3dGraphicsRootSignature = CreateGraphicsRootSignature(device.Get());

	CInstancingShader* shader{ new CInstancingShader{} };
	shader->CreateShader(device.Get(), m_pd3dGraphicsRootSignature.Get());
	shader->BuildObjects(device.Get(), commandList);
	m_pShaders.push_back(shader);
	shader = new CEnemyBoxShader{};
	shader->CreateShader(device.Get(), m_pd3dGraphicsRootSignature.Get());
	shader->BuildObjects(device.Get(), commandList);
	m_pShaders.push_back(shader);
}

void CScene::ReleaseObjects() {
	if (m_pd3dGraphicsRootSignature) m_pd3dGraphicsRootSignature->Release();
	for (auto& it : m_pShaders) {
		it->ReleaseShaderVariables();
		it->ReleaseObjects();
		delete it;
	}
	m_pShaders.clear();
}

bool CScene::ProcessInput(UCHAR* pKeysBuffer) {
	return false;
}
bool CScene::ProcessInput() {
	return false;
}

void CScene::AnimateObjects(float fTimeElapsed) {
	for (auto& it : m_pShaders)
		it->AnimateObjects(fTimeElapsed);
}

void CScene::Render(const ComPtr<ID3D12GraphicsCommandList>& pd3dCommandList, CCamera* pCamera) {
	pCamera->SetViewportsAndScissorRects(pd3dCommandList.Get());
	pd3dCommandList->SetGraphicsRootSignature(m_pd3dGraphicsRootSignature.Get());

	if (pCamera) pCamera->UpdateShaderVariables(pd3dCommandList.Get());

	for (auto& it : m_pShaders)
		it->Render(pd3dCommandList.Get(), pCamera);
}

bool CScene::ProcessCollision(CAirplanePlayer* player){

	CEnemyBoxShader* shader{
		reinterpret_cast<CEnemyBoxShader*>(m_pShaders[1]) };
	
	bool bIsCollide{ false };

	for (auto& bullet : player->GetBulletList()) {
		for (auto& box : shader->GetGameObject()) {
			CEnemyBox* enemy{ reinterpret_cast<CEnemyBox*>(box) };
			CBullet* bul{ reinterpret_cast<CBullet*>(bullet) };
			if (enemy->isCollision(bul->GetBoundingBox())) {
				enemy->SetLive(false);
				bul->SetLive(false);
				bIsCollide = true;
			}
		}
	}
	return bIsCollide;
}

ID3D12RootSignature* CScene::CreateGraphicsRootSignature(ID3D12Device* device) {
	ID3D12RootSignature* pd3dGraphicsRootSignature = NULL;
	D3D12_ROOT_PARAMETER pd3dRootParameters[3];
	pd3dRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[0].Constants.Num32BitValues = 16;
	pd3dRootParameters[0].Constants.ShaderRegister = 0; //b0: Player
	pd3dRootParameters[0].Constants.RegisterSpace = 0;
	pd3dRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	pd3dRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	pd3dRootParameters[1].Constants.Num32BitValues = 32;
	pd3dRootParameters[1].Constants.ShaderRegister = 1; //b1: Camera
	pd3dRootParameters[1].Constants.RegisterSpace = 0;
	pd3dRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
	pd3dRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	pd3dRootParameters[2].Descriptor.ShaderRegister = 0; //t0
	pd3dRootParameters[2].Descriptor.RegisterSpace = 0;
	pd3dRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	D3D12_ROOT_SIGNATURE_FLAGS d3dRootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

	D3D12_ROOT_SIGNATURE_DESC d3dRootSignatureDesc;
	::ZeroMemory(&d3dRootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	d3dRootSignatureDesc.NumParameters = _countof(pd3dRootParameters);
	d3dRootSignatureDesc.pParameters = pd3dRootParameters;
	d3dRootSignatureDesc.NumStaticSamplers = 0;
	d3dRootSignatureDesc.pStaticSamplers = NULL;
	d3dRootSignatureDesc.Flags = d3dRootSignatureFlags;
	ID3DBlob* pd3dSignatureBlob = NULL;
	ID3DBlob* pd3dErrorBlob = NULL;

	D3D12SerializeRootSignature(&d3dRootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		&pd3dSignatureBlob, &pd3dErrorBlob);
	device->CreateRootSignature(0, pd3dSignatureBlob->GetBufferPointer(),
		pd3dSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void
			**)&pd3dGraphicsRootSignature);

	if (pd3dSignatureBlob) pd3dSignatureBlob->Release();
	if (pd3dErrorBlob) pd3dErrorBlob->Release();
	return(pd3dGraphicsRootSignature);
}

ID3D12RootSignature* CScene::GetGraphicsRootSignature() {
	return m_pd3dGraphicsRootSignature.Get();
}

void CScene::ReleaseUploadBuffers() {
	for (auto& it : m_pShaders)
		it->ReleaseUploadBuffers();
}

CGameObject* CScene::pickingObjects(const XMFLOAT3& rayDirOrigin, const XMFLOAT3& rayDir){
	CEnemyBoxShader* boxShader{ reinterpret_cast<CEnemyBoxShader*>(m_pShaders[1]) };
	return boxShader->IsPickingObject(rayDirOrigin, rayDir);
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	return false;
}
bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, WPARAM wParam, LPARAM lParam) {
	return false;
}