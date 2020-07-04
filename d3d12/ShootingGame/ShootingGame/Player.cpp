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
	if (dwDirection)
	{
		XMFLOAT3 xmf3Shift{ XMFLOAT3(0, 0, 0) };

		//화살표 키 ‘↑’를 누르면 로컬 z-축 방향으로 이동(전진)한다. ‘↓’를 누르면 반대 방향으로 이동한다. 
		if (dwDirection & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, fDistance);
		if (dwDirection & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -fDistance);
		//화살표 키 ‘→’를 누르면 로컬 x-축 방향으로 이동한다. ‘←’를 누르면 반대 방향으로 이동한다. 
		if (dwDirection & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, fDistance);
		if (dwDirection & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -fDistance);
		//‘Page Up’을 누르면 로컬 y-축 방향으로 이동한다. ‘Page Down’을 누르면 반대 방향으로 이동한다. 
		if (dwDirection & DIR_UP) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, fDistance);
		if (dwDirection & DIR_DOWN) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, -fDistance);

		//플레이어를 현재 위치 벡터에서 xmf3Shift 벡터만큼 이동한다. 
		Move(xmf3Shift, bUpdateVelocity);
	}
}

void CPlayer::Move(const XMFLOAT3& xmf3Shift, bool bUpdateVelocity) {
	//bUpdateVelocity가 참이면 플레이어를 이동하지 않고 속도 벡터를 변경한다. 
	if (bUpdateVelocity)
	{
		//플레이어의 속도 벡터를 xmf3Shift 벡터만큼 변경한다. 
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	}
	else
	{
		//플레이어를 현재 위치 벡터에서 xmf3Shift 벡터만큼 이동한다. 
		m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);
		//플레이어의 위치가 변경되었으므로 카메라의 위치도 xmf3Shift 벡터만큼 이동한다. 
		if (m_pCamera) m_pCamera->Move(xmf3Shift);
	}

	if (m_xmf3Position.x > 200.0f || m_xmf3Position.x < -200.0f ||
		m_xmf3Position.y > 200.0f || m_xmf3Position.y < -200.0f ||
		m_xmf3Position.z > 300.0f || m_xmf3Position.z < -300.0f)
		m_xmf3Position = XMFLOAT3{ 0.0f, 0.0f, 0.0f };
}

//플레이어를 로컬 x-축, y-축, z-축을 중심으로 회전한다. 
void CPlayer::Rotate(float x, float y, float z) {
	/*로컬 x-축을 중심으로 회전하는 것은 고개를 앞뒤로 숙이는 동작에 해당한다. 그러므로 x-축을 중심으로 회전하는
	각도는 -89.0~+89.0도 사이로 제한한다. x는 현재의 m_fPitch에서 실제 회전하는 각도이므로 x만큼 회전한 다음
	Pitch가 +89도 보다 크거나 -89도 보다 작으면 m_fPitch가 +89도 또는 -89도가 되도록 회전각도(x)를 수정한다.*/
	if (x != 0.0f)
	{
		XMMATRIX xmmtxRotate{ XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(x)) };
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
	}
	if (y != 0.0f)
	{
		XMMATRIX xmmtxRotate{ XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y)) };
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
	}
	if (z != 0.0f)
	{
		XMMATRIX xmmtxRotate{ XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look), XMConvertToRadians(z)) };
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
	}

	//카메라를 x, y, z 만큼 회전한다. 플레이어를 회전하면 카메라가 회전하게 된다. 
	m_pCamera->Rotate(x, y, z);

	/*플레이어를 회전한다. 1인칭 카메라 또는 3인칭 카메라에서 플레이어의 회전은 로컬 y-축에서만 일어난다. 플레이어
	의 로컬 y-축(Up 벡터)을 기준으로 로컬 z-축(Look 벡터)와 로컬 x-축(Right 벡터)을 회전시킨다. 기본적으로 Up 벡
	터를 기준으로 회전하는 것은 플레이어가 똑바로 서있는 것을 가정한다는 의미이다.*/
	if (y != 0.0f)
	{
		XMMATRIX xmmtxRotate{ XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y)) };
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
	}

	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
}

//이 함수는 매 프레임마다 호출된다. 플레이어의 속도 벡터에 중력과 마찰력 등을 적용한다. 
void CPlayer::Update(float fTimeElapsed) {
	/*플레이어의 속도 벡터를 중력 벡터와 더한다. 중력 벡터에 fTimeElapsed를 곱하는 것은 중력을 시간에 비례하도록
	적용한다는 의미이다.*/
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Gravity, fTimeElapsed, false));

	/*플레이어의 속도 벡터의 XZ-성분의 크기를 구한다. 이것이 XZ-평면의 최대 속력보다 크면 속도 벡터의 x와 z-방향
	성분을 조정한다.*/

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

/*플레이어의 위치와 회전축으로부터 월드 변환 행렬을 생성하는 함수이다. 플레이어의 Right 벡터가 월드 변환 행렬
의 첫 번째 행 벡터, Up 벡터가 두 번째 행 벡터, Look 벡터가 세 번째 행 벡터, 플레이어의 위치 벡터가 네 번째 행
벡터가 된다.*/
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

///////////////////////////////////////////////////////////////////////////////////////////////

CAirplanePlayer::CAirplanePlayer(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList,
	ID3D12RootSignature* pd3dGraphicsRootSignature)
{
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

	//3인칭 카메라의 지연 효과를 설정한다. 값을 0.25f 대신에 0.0f와 1.0f로 설정한 결과를 비교하기 바란다. 
	m_pCamera->SetTimeLag(0.25f);
	m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, -50.0f));
	m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
	m_pCamera->SetViewport(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT, 0.0f, 1.0f);
	m_pCamera->SetScissorRect(0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT);

	//플레이어를 위한 셰이더 변수를 생성한다. 
	CreateShaderVariables(pd3dDevice, pd3dCommandList);

	//플레이어의 위치를 설정한다. 
	SetPosition(XMFLOAT3(0.0f, 0.0f, -50.0f));

	//플레이어(비행기) 메쉬를 렌더링할 때 사용할 셰이더를 생성한다.
	CPlayerShader* pShader{ new CPlayerShader() };
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

/*3인칭 카메라일 때 플레이어 메쉬를 로컬 x-축을 중심으로 +90도 회전하고 렌더링한다. 왜냐하면 비행기 모델 메쉬
는 다음 그림과 같이 y-축 방향이 비행기의 앞쪽이 되도록 모델링이 되었기 때문이다. 그리고 이 메쉬를 카메라의 z- 축
방향으로 향하도록 그릴 것이기 때문이다.*/
void CAirplanePlayer::OnPrepareRender()
{
	CPlayer::OnPrepareRender();

	//비행기 모델을 그리기 전에 x-축으로 90도 회전한다. 
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
