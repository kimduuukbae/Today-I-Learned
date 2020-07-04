#include "stdafx.h"
#include "Camera.h"
#include "Player.h"
CCamera::CCamera() {
	viewMatrix = Matrix4x4::Identity();
	projectMatrix = Matrix4x4::Identity();
	position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	right = XMFLOAT3(1.0f, 0.0f, 0.0f);
	look = XMFLOAT3(0.0f, 0.0f, 1.0f);
	up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	pitch = 0.0f;
	roll = 0.0f;
	yaw = 0.0f;
	offset = XMFLOAT3(0.0f, 0.0f, 0.0f);
	timeLag = 0.0f;
	lookAtWorld = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mode = 0x00;
	m_pPlayer = nullptr;
	viewport = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
	scissorRect = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
}
CCamera::CCamera(CCamera* pCamera){
	if (pCamera)
		*this = *pCamera;
	else 
		*this = CCamera{};
}


CCamera::~CCamera() {

}
void CCamera::CreateShaderVariables(ID3D12Device* device, ID3D12GraphicsCommandList* commandList){

}

void CCamera::ReleaseShaderVariables(){

}

void CCamera::UpdateShaderVariables(ID3D12GraphicsCommandList* commandList){
	XMFLOAT4X4 view{};
	XMStoreFloat4x4(&view, XMMatrixTranspose(XMLoadFloat4x4(&viewMatrix)));
	commandList->SetGraphicsRoot32BitConstants(1, 16, &view, 0);
	XMFLOAT4X4 project{};
	XMStoreFloat4x4(&project, XMMatrixTranspose(XMLoadFloat4x4(&projectMatrix)));
	commandList->SetGraphicsRoot32BitConstants(1, 16, &project, 16);
}

void CCamera::GenerateViewMatrix() {
	viewMatrix = Matrix4x4::LookAtLH(position, lookAtWorld, up);
}

void CCamera::GenerateViewMatrix(XMFLOAT3 pos, XMFLOAT3 lookAt, XMFLOAT3 fup){
	position = pos;
	lookAtWorld = lookAt;
	up = fup;

	GenerateViewMatrix();
}

void CCamera::RegenerateViewMatrix(){
	look = Vector3::Normalize(look);
	right = Vector3::CrossProduct(up, look, true);
	up = Vector3::CrossProduct(look, right, true);

	// 카메라 변환행렬
	viewMatrix._11 = right.x; viewMatrix._12 = up.x; viewMatrix._13 =
		look.x;
	viewMatrix._21 = right.y; viewMatrix._22 = up.y; viewMatrix._23 =
		look.y;
	viewMatrix._31 = right.z; viewMatrix._32 = up.z; viewMatrix._33 =
		look.z;
	viewMatrix._41 = -Vector3::DotProduct(position, right);
	viewMatrix._42 = -Vector3::DotProduct(position, up);
	viewMatrix._43 = -Vector3::DotProduct(position, look);

	// 카메라 변환행렬의 _41,_42,_43 은 -(pos dot )
}

void CCamera::GenerateProjectionMatrix(float fNearPlaneDistance, float fFarPlaneDistance, float fAspectRatio, float fFOVAngle){
	projectMatrix = Matrix4x4::PerspectiveFovLH(XMConvertToRadians(fFOVAngle), fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
}

void CCamera::SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ, float fMaxZ){
	viewport.TopLeftX = float(xTopLeft);
	viewport.TopLeftY = float(yTopLeft);
	viewport.Width = float(nWidth);
	viewport.Height = float(nHeight);
	viewport.MinDepth = fMinZ;
	viewport.MaxDepth = fMaxZ;
}

void CCamera::SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom){
	scissorRect.left = xLeft;
	scissorRect.top = yTop;
	scissorRect.right = xRight;
	scissorRect.bottom = yBottom;
}

void CCamera::SetViewportsAndScissorRects(ID3D12GraphicsCommandList* pd3dCommandList){
	pd3dCommandList->RSSetViewports(1, &viewport);
	pd3dCommandList->RSSetScissorRects(1, &scissorRect);
}


void CCamera::Move(const XMFLOAT3& shift) {
	position.x += shift.x;
	position.y += shift.y;
	position.z += shift.z;
}

CSpaceShipCamera::CSpaceShipCamera(CCamera* pCamera) : CCamera(pCamera){
	mode = SPACESHIP_CAMERA;
}

void CSpaceShipCamera::Rotate(float x, float y, float z){
	if (m_pPlayer && (x != 0.0f)){
		//플레이어의 로컬 x-축에 대한 x 각도의 회전 행렬을 계산한다. 
		XMFLOAT3 xmf3Right = m_pPlayer->GetRightVector();
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Right),
			XMConvertToRadians(x));
		//카메라의 로컬 x-축, y-축, z-축을 회전한다. 
		right = Vector3::TransformNormal(right, xmmtxRotate);
		up = Vector3::TransformNormal(up, xmmtxRotate);
		look = Vector3::TransformNormal(look, xmmtxRotate);
		/*카메라의 위치 벡터에서 플레이어의 위치 벡터를 뺀다. 결과는 플레이어 위치를 기준(원점)으로 한 카메라의 위치
		벡터이다.*/
		position = Vector3::Subtract(position, m_pPlayer->GetPosition());
		//플레이어의 위치를 중심으로 카메라의 위치 벡터(플레이어를 기준으로 한)를 회전한다. 
		position = Vector3::TransformCoord(position, xmmtxRotate);
		//회전시킨 카메라의 위치 벡터에 플레이어의 위치를 더하여 카메라의 위치 벡터를 구한다.
		position = Vector3::Add(position, m_pPlayer->GetPosition()); // -> 자전해야하므로 
	}
	if (m_pPlayer && (y != 0.0f))
	{
		XMFLOAT3 xmf3Up = m_pPlayer->GetUpVector();
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up),
			XMConvertToRadians(y));
		right = Vector3::TransformNormal(right, xmmtxRotate);
		up = Vector3::TransformNormal(up, xmmtxRotate);
		look = Vector3::TransformNormal(look, xmmtxRotate);
		position = Vector3::Subtract(position, m_pPlayer->GetPosition());
		position = Vector3::TransformCoord(position, xmmtxRotate);
		position = Vector3::Add(position, m_pPlayer->GetPosition());
	}
	if (m_pPlayer && (z != 0.0f))
	{
		XMFLOAT3 xmf3Look = m_pPlayer->GetLookVector();
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Look),
			XMConvertToRadians(z));
		right = Vector3::TransformNormal(right, xmmtxRotate);
		up = Vector3::TransformNormal(up, xmmtxRotate);
		look = Vector3::TransformNormal(look, xmmtxRotate);
		position = Vector3::Subtract(position, m_pPlayer->GetPosition());
		position = Vector3::TransformCoord(position, xmmtxRotate);
		position = Vector3::Add(position, m_pPlayer->GetPosition());
	}
}

CFirstPersonCamera::CFirstPersonCamera(CCamera* pCamera) : CCamera(pCamera){
	mode = FIRST_PERSON_CAMERA;
	if (pCamera) {
		if (pCamera->GetMode() == SPACESHIP_CAMERA) {
			up = XMFLOAT3(0.0f, 1.0f, 0.0f);
			right.y = 0.0f;
			look.y = 0.0f;
			right = Vector3::Normalize(right);
			look = Vector3::Normalize(look);
		}
	}
}

void CFirstPersonCamera::Rotate(float x, float y, float z){
	if (x != 0.0f)
	{
		//카메라의 로컬 x-축을 기준으로 회전하는 행렬을 생성한다. 사람의 경우 고개를 끄떡이는 동작이다. 
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&right), 
		XMConvertToRadians(x));
		//카메라의 로컬 x-축, y-축, z-축을 회전 행렬을 사용하여 회전한다. 
		look = Vector3::TransformNormal(look, xmmtxRotate);
		up = Vector3::TransformNormal(up, xmmtxRotate);
		right = Vector3::TransformNormal(right, xmmtxRotate);
	}
	if (m_pPlayer && (y != 0.0f))
	{
		//플레이어의 로컬 y-축을 기준으로 회전하는 행렬을 생성한다. 
		XMFLOAT3 xmf3Up = m_pPlayer->GetUpVector();
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up),
			XMConvertToRadians(y));
		//카메라의 로컬 x-축, y-축, z-축을 회전 행렬을 사용하여 회전한다. 
		look = Vector3::TransformNormal(look, xmmtxRotate);
		up = Vector3::TransformNormal(up, xmmtxRotate);
		right = Vector3::TransformNormal(right, xmmtxRotate);
	}
	if (m_pPlayer && (z != 0.0f))
	{
		//플레이어의 로컬 z-축을 기준으로 회전하는 행렬을 생성한다
		XMFLOAT3 xmf3Look = m_pPlayer->GetLookVector();
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Look),
			XMConvertToRadians(z));
		//카메라의 위치 벡터를 플레이어 좌표계로 표현한다(오프셋 벡터).
		position = Vector3::Subtract(position, m_pPlayer->GetPosition());
		//오프셋 벡터 벡터를 회전한다.
		position = Vector3::TransformCoord(position, xmmtxRotate);
		//회전한 카메라의 위치를 월드 좌표계로 표현한다. 
		position = Vector3::Add(position, m_pPlayer->GetPosition());
		//카메라의 로컬 x-축, y-축, z-축을 회전한다. 
		look = Vector3::TransformNormal(look, xmmtxRotate);
		up = Vector3::TransformNormal(up, xmmtxRotate);
		right = Vector3::TransformNormal(right, xmmtxRotate);
	}
}

CThirdPersonCamera::CThirdPersonCamera(CCamera* pCamera){
	mode = THIRD_PERSON_CAMERA;
	if (pCamera) {
		if (pCamera->GetMode() == SPACESHIP_CAMERA) {
			up = XMFLOAT3(0.0f, 1.0f, 0.0f);
			right.y = 0.0f;
			look.y = 0.0f;
			right = Vector3::Normalize(right);
			look = Vector3::Normalize(look);
		}
	}
}

void CThirdPersonCamera::Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed){
	if (m_pPlayer) {
		XMFLOAT4X4 xmf4x4Rotate = Matrix4x4::Identity();
		XMFLOAT3 xmf3Right = m_pPlayer->GetRightVector();
		XMFLOAT3 xmf3Up = m_pPlayer->GetUpVector();
		XMFLOAT3 xmf3Look = m_pPlayer->GetLookVector();
		//플레이어의 로컬 x-축, y-축, z-축 벡터로부터 회전 행렬(플레이어와 같은 방향을 나타내는 행렬)을 생성한다. 
		xmf4x4Rotate._11 = xmf3Right.x; xmf4x4Rotate._21 = xmf3Up.x; xmf4x4Rotate._31 =
		xmf3Look.x;
		xmf4x4Rotate._12 = xmf3Right.y; xmf4x4Rotate._22 = xmf3Up.y; xmf4x4Rotate._32 =
			xmf3Look.y;
		xmf4x4Rotate._13 = xmf3Right.z; xmf4x4Rotate._23 = xmf3Up.z; xmf4x4Rotate._33 =
			xmf3Look.z;
		//카메라 오프셋 벡터를 회전 행렬로 변환(회전)한다. 
		XMFLOAT3 xmf3Offset = Vector3::TransformCoord(offset, xmf4x4Rotate);
		//회전한 카메라의 위치는 플레이어의 위치에 회전한 카메라 오프셋 벡터를 더한 것이다.
		XMFLOAT3 xmf3Position = Vector3::Add(m_pPlayer->GetPosition(), xmf3Offset);
		//현재의 카메라의 위치에서 회전한 카메라의 위치까지의 방향과 거리를 나타내는 벡터이다. 
		XMFLOAT3 xmf3Direction = Vector3::Subtract(xmf3Position, position);
		float fLength = Vector3::Length(xmf3Direction);
		xmf3Direction = Vector3::Normalize(xmf3Direction);
		/*3인칭 카메라의 래그(Lag)는 플레이어가 회전하더라도 카메라가 동시에 따라서 회전하지 않고 약간의 시차를 두고
		회전하는 효과를 구현하기 위한 것이다. m_fTimeLag가 1보다 크면 fTimeLagScale이 작아지고 실제 회전(이동)이 적
		게 일어날 것이다. m_fTimeLag가 0이 아닌 경우 fTimeElapsed를 곱하고 있으므로 3인칭 카메라는 1초의 시간동안
		(1.0f / m_fTimeLag)의 비율만큼 플레이어의 회전을 따라가게 될 것이다.*/
		float fTimeLagScale = (timeLag) ? fTimeElapsed * (1.0f / timeLag) : 1.0f;
		float fDistance = fLength * fTimeLagScale;
		if (fDistance > fLength) fDistance = fLength;
		if (fLength < 0.01f) fDistance = fLength;
		if (fDistance > 0)
		{
			//실제로 카메라를 회전하지 않고 이동을 한다(회전의 각도가 작은 경우 회전 이동은 선형 이동과 거의 같다).
			position = Vector3::Add(position, xmf3Direction, fDistance);
			//카메라가 플레이어를 바라보도록 한다. 
			SetLookAt(xmf3LookAt);
		}
	}
}

void CThirdPersonCamera::SetLookAt(XMFLOAT3& vLookAt){
	XMFLOAT4X4 mtxLookAt = Matrix4x4::LookAtLH(position, vLookAt,
		m_pPlayer->GetUpVector());
	//카메라 변환 행렬에서 카메라의 x-축, y-축, z-축을 구한다. 
	right = XMFLOAT3(mtxLookAt._11, mtxLookAt._21, mtxLookAt._31);
	up = XMFLOAT3(mtxLookAt._12, mtxLookAt._22, mtxLookAt._32);
	look = XMFLOAT3(mtxLookAt._13, mtxLookAt._23, mtxLookAt._33);
}
