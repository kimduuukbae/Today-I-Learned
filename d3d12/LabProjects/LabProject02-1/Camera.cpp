#include "stdafx.h"
#include "Camera.h"

using namespace DirectX;

CCamera::~CCamera(){
	if (m_pViewport)
		delete m_pViewport;
}

XMVECTOR CCamera::CameraTransform(FXMVECTOR& f3World) {
	XMFLOAT4 eye{ viewMatrix.m[3] };
	XMFLOAT4 at{ eye.x, eye.y, eye.z + 1.0f, 0.0f };
	XMFLOAT4 up{ 0.0f, 1.0f, 0.0f, 0.0f };

	XMVECTOR eyeVector{ XMLoadFloat4(&eye) };
	XMVECTOR atVector{ XMLoadFloat4(&at) };
	XMVECTOR upVector{ XMLoadFloat4(&up) };

	return XMVector3TransformCoord(f3World, XMMatrixLookAtLH(eyeVector, atVector, upVector));
}

XMVECTOR CCamera::ProjectionTransform(FXMVECTOR& f3Camera){
	XMFLOAT3 project{ VectorToF3(f3Camera) };
	XMFLOAT3 camera{ VectorToF3(f3Camera) };
	//XMMatrixPerspectiveFovLH(m_fFOVAngle, m_fAspectRatio, m_fProjectRectDistance);
	if (camera.z != 0.0f) {
		// 카메라의 시야각이 90도가 아닌 경우 투영 사각형까지의 거리를 곱한다.
		project.x = float((camera.x * m_fProjectRectDistance) /
			(m_fAspectRatio * camera.z));
		project.y = float((camera.y * m_fProjectRectDistance) /
			camera.z);
		// 투영 좌표계는 2차원이므로 z 좌표에 카메라 좌표 z좌표를 저장
		project.z = camera.z;
	}
	//return XMLoadFloat3(&project);

	return XMVector3TransformCoord(f3Camera, XMMatrixPerspectiveFovLH(m_fFOVAngle
		, m_fAspectRatio, 0.1f, 1000.0f));
}

XMVECTOR CCamera::ScreenTransform(FXMVECTOR& f3Projection){
	// 투영 좌표계의 점을 화면 좌표계로 변환하는 함수
	XMFLOAT3 screen{};
	DirectX::XMStoreFloat3(&screen, f3Projection);
	float fHalfWidth = { m_pViewport->m_nWidth * 0.5f };
	float fHalfHeight = { m_pViewport->m_nHeight * 0.5f };
	screen.x = (screen.x * fHalfWidth) + m_pViewport->m_nLeft + fHalfWidth;
	screen.y = (-screen.y * fHalfHeight) + m_pViewport->m_nTop + fHalfHeight;
	return XMLoadFloat3(&screen);
}

void CCamera::SetViewport(int nLeft, int nTop, int nWidth, int nHeight){
	m_pViewport = new CViewport(nLeft, nTop, nWidth, nHeight);
	m_fAspectRatio = float(nWidth) / float(nHeight);
}

void CCamera::SetFOVAngle(float fFOVAngle){
	m_fFOVAngle = fFOVAngle;
	m_fProjectRectDistance = 1.0f / std::tanf(degreeToRadian(fFOVAngle * 0.5f));
}

void CCamera::Move(float x, float y, float z){
	viewMatrix._41 += x;
	viewMatrix._42 += y;
	viewMatrix._43 += z;
}

void CCamera::Rotate(float fPitch, float fYaw, float fRoll){
	XMFLOAT4X4& t{ viewMatrix };
	XMFLOAT3X3 f{ t._11, t._12, t._13,
				 t._21, t._22, t._23,
				 t._31, t._32, t._33 };
	XMMATRIX mat{ XMMatrixMultiply(XMLoadFloat3x3(&f), XMMatrixRotationRollPitchYaw(fRoll, fPitch, fYaw)) };
	::memcpy(&viewMatrix, &mat, sizeof(float) * 12);
}

void CCamera::SetPosition(float x, float y, float z) {
	viewMatrix._41 = x;
	viewMatrix._42 = y;
	viewMatrix._43 = z;
}

void CCamera::SetRotation(float fPitch, float fYaw, float fRoll) {
	XMMATRIX mat{ XMMatrixRotationRollPitchYaw(fRoll, fPitch, fYaw) };
	::memcpy(&viewMatrix, &mat, sizeof(float) * 12);
}