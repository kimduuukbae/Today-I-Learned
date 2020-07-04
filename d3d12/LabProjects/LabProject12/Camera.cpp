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

	// ī�޶� ��ȯ���
	viewMatrix._11 = right.x; viewMatrix._12 = up.x; viewMatrix._13 =
		look.x;
	viewMatrix._21 = right.y; viewMatrix._22 = up.y; viewMatrix._23 =
		look.y;
	viewMatrix._31 = right.z; viewMatrix._32 = up.z; viewMatrix._33 =
		look.z;
	viewMatrix._41 = -Vector3::DotProduct(position, right);
	viewMatrix._42 = -Vector3::DotProduct(position, up);
	viewMatrix._43 = -Vector3::DotProduct(position, look);

	// ī�޶� ��ȯ����� _41,_42,_43 �� -(pos dot )
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
		//�÷��̾��� ���� x-�࿡ ���� x ������ ȸ�� ����� ����Ѵ�. 
		XMFLOAT3 xmf3Right = m_pPlayer->GetRightVector();
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Right),
			XMConvertToRadians(x));
		//ī�޶��� ���� x-��, y-��, z-���� ȸ���Ѵ�. 
		right = Vector3::TransformNormal(right, xmmtxRotate);
		up = Vector3::TransformNormal(up, xmmtxRotate);
		look = Vector3::TransformNormal(look, xmmtxRotate);
		/*ī�޶��� ��ġ ���Ϳ��� �÷��̾��� ��ġ ���͸� ����. ����� �÷��̾� ��ġ�� ����(����)���� �� ī�޶��� ��ġ
		�����̴�.*/
		position = Vector3::Subtract(position, m_pPlayer->GetPosition());
		//�÷��̾��� ��ġ�� �߽����� ī�޶��� ��ġ ����(�÷��̾ �������� ��)�� ȸ���Ѵ�. 
		position = Vector3::TransformCoord(position, xmmtxRotate);
		//ȸ����Ų ī�޶��� ��ġ ���Ϳ� �÷��̾��� ��ġ�� ���Ͽ� ī�޶��� ��ġ ���͸� ���Ѵ�.
		position = Vector3::Add(position, m_pPlayer->GetPosition()); // -> �����ؾ��ϹǷ� 
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
		//ī�޶��� ���� x-���� �������� ȸ���ϴ� ����� �����Ѵ�. ����� ��� ���� �����̴� �����̴�. 
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&right), 
		XMConvertToRadians(x));
		//ī�޶��� ���� x-��, y-��, z-���� ȸ�� ����� ����Ͽ� ȸ���Ѵ�. 
		look = Vector3::TransformNormal(look, xmmtxRotate);
		up = Vector3::TransformNormal(up, xmmtxRotate);
		right = Vector3::TransformNormal(right, xmmtxRotate);
	}
	if (m_pPlayer && (y != 0.0f))
	{
		//�÷��̾��� ���� y-���� �������� ȸ���ϴ� ����� �����Ѵ�. 
		XMFLOAT3 xmf3Up = m_pPlayer->GetUpVector();
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up),
			XMConvertToRadians(y));
		//ī�޶��� ���� x-��, y-��, z-���� ȸ�� ����� ����Ͽ� ȸ���Ѵ�. 
		look = Vector3::TransformNormal(look, xmmtxRotate);
		up = Vector3::TransformNormal(up, xmmtxRotate);
		right = Vector3::TransformNormal(right, xmmtxRotate);
	}
	if (m_pPlayer && (z != 0.0f))
	{
		//�÷��̾��� ���� z-���� �������� ȸ���ϴ� ����� �����Ѵ�
		XMFLOAT3 xmf3Look = m_pPlayer->GetLookVector();
		XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Look),
			XMConvertToRadians(z));
		//ī�޶��� ��ġ ���͸� �÷��̾� ��ǥ��� ǥ���Ѵ�(������ ����).
		position = Vector3::Subtract(position, m_pPlayer->GetPosition());
		//������ ���� ���͸� ȸ���Ѵ�.
		position = Vector3::TransformCoord(position, xmmtxRotate);
		//ȸ���� ī�޶��� ��ġ�� ���� ��ǥ��� ǥ���Ѵ�. 
		position = Vector3::Add(position, m_pPlayer->GetPosition());
		//ī�޶��� ���� x-��, y-��, z-���� ȸ���Ѵ�. 
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
		//�÷��̾��� ���� x-��, y-��, z-�� ���ͷκ��� ȸ�� ���(�÷��̾�� ���� ������ ��Ÿ���� ���)�� �����Ѵ�. 
		xmf4x4Rotate._11 = xmf3Right.x; xmf4x4Rotate._21 = xmf3Up.x; xmf4x4Rotate._31 =
		xmf3Look.x;
		xmf4x4Rotate._12 = xmf3Right.y; xmf4x4Rotate._22 = xmf3Up.y; xmf4x4Rotate._32 =
			xmf3Look.y;
		xmf4x4Rotate._13 = xmf3Right.z; xmf4x4Rotate._23 = xmf3Up.z; xmf4x4Rotate._33 =
			xmf3Look.z;
		//ī�޶� ������ ���͸� ȸ�� ��ķ� ��ȯ(ȸ��)�Ѵ�. 
		XMFLOAT3 xmf3Offset = Vector3::TransformCoord(offset, xmf4x4Rotate);
		//ȸ���� ī�޶��� ��ġ�� �÷��̾��� ��ġ�� ȸ���� ī�޶� ������ ���͸� ���� ���̴�.
		XMFLOAT3 xmf3Position = Vector3::Add(m_pPlayer->GetPosition(), xmf3Offset);
		//������ ī�޶��� ��ġ���� ȸ���� ī�޶��� ��ġ������ ����� �Ÿ��� ��Ÿ���� �����̴�. 
		XMFLOAT3 xmf3Direction = Vector3::Subtract(xmf3Position, position);
		float fLength = Vector3::Length(xmf3Direction);
		xmf3Direction = Vector3::Normalize(xmf3Direction);
		/*3��Ī ī�޶��� ����(Lag)�� �÷��̾ ȸ���ϴ��� ī�޶� ���ÿ� ���� ȸ������ �ʰ� �ణ�� ������ �ΰ�
		ȸ���ϴ� ȿ���� �����ϱ� ���� ���̴�. m_fTimeLag�� 1���� ũ�� fTimeLagScale�� �۾����� ���� ȸ��(�̵�)�� ��
		�� �Ͼ ���̴�. m_fTimeLag�� 0�� �ƴ� ��� fTimeElapsed�� ���ϰ� �����Ƿ� 3��Ī ī�޶�� 1���� �ð�����
		(1.0f / m_fTimeLag)�� ������ŭ �÷��̾��� ȸ���� ���󰡰� �� ���̴�.*/
		float fTimeLagScale = (timeLag) ? fTimeElapsed * (1.0f / timeLag) : 1.0f;
		float fDistance = fLength * fTimeLagScale;
		if (fDistance > fLength) fDistance = fLength;
		if (fLength < 0.01f) fDistance = fLength;
		if (fDistance > 0)
		{
			//������ ī�޶� ȸ������ �ʰ� �̵��� �Ѵ�(ȸ���� ������ ���� ��� ȸ�� �̵��� ���� �̵��� ���� ����).
			position = Vector3::Add(position, xmf3Direction, fDistance);
			//ī�޶� �÷��̾ �ٶ󺸵��� �Ѵ�. 
			SetLookAt(xmf3LookAt);
		}
	}
}

void CThirdPersonCamera::SetLookAt(XMFLOAT3& vLookAt){
	XMFLOAT4X4 mtxLookAt = Matrix4x4::LookAtLH(position, vLookAt,
		m_pPlayer->GetUpVector());
	//ī�޶� ��ȯ ��Ŀ��� ī�޶��� x-��, y-��, z-���� ���Ѵ�. 
	right = XMFLOAT3(mtxLookAt._11, mtxLookAt._21, mtxLookAt._31);
	up = XMFLOAT3(mtxLookAt._12, mtxLookAt._22, mtxLookAt._32);
	look = XMFLOAT3(mtxLookAt._13, mtxLookAt._23, mtxLookAt._33);
}
