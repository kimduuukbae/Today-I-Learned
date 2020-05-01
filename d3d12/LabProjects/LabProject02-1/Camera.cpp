#include "stdafx.h"
#include "Camera.h"

using namespace DirectX;

CCamera::~CCamera(){
	if (m_pViewport)
		delete m_pViewport;
}

XMVECTOR CCamera::CameraTransform(FXMVECTOR& f3World) {
	XMFLOAT3 curCameraPosition{};
	XMStoreFloat3(&curCameraPosition, f3World);

	curCameraPosition.x -= position.x;
	curCameraPosition.y -= position.y;
	curCameraPosition.z -= position.z;

	// world 좌표로 카메라 이동
	float fPitch{ degreeToRadian(-rotation.x) };
	float fYaw{ degreeToRadian(-rotation.y) };
	float fRoll { degreeToRadian(-rotation.z) };

	XMFLOAT3 f3Rotated{ curCameraPosition };

	if (fPitch != 0.0f) {
		f3Rotated.y = curCameraPosition.y * std::cosf(fPitch) - curCameraPosition.z * std::sinf(fPitch);
		f3Rotated.z = curCameraPosition.y * std::sinf(fPitch) + curCameraPosition.z * std::cosf(fPitch);
		curCameraPosition.y = f3Rotated.y;
		curCameraPosition.z = f3Rotated.z;
	}
	if (fYaw != 0.0f) {
		f3Rotated.x = curCameraPosition.x * std::cosf(fYaw) + curCameraPosition.z * std::sinf(fYaw);
		f3Rotated.z = -curCameraPosition.x * std::sinf(fYaw) + curCameraPosition.z * std::cosf(fYaw);
		curCameraPosition.x = f3Rotated.x;
		curCameraPosition.z = f3Rotated.z;
	}
	if (fRoll != 0.0f) {
		f3Rotated.x = curCameraPosition.x * std::cosf(fRoll) - curCameraPosition.y * std::sinf(fRoll);
		f3Rotated.y = curCameraPosition.x * std::sinf(fRoll) + curCameraPosition.y * std::cosf(fRoll);
		curCameraPosition.x = f3Rotated.x;
		curCameraPosition.y = f3Rotated.y;
	}

	//XMMATRIX tRotation{ XMMatrixRotationRollPitchYaw(fPitch, fYaw, fRoll) };
	// 카메라를 축에 맞춰 회전 행렬 반환
	// 위치와 회전 행렬 곱
	return XMLoadFloat3(&curCameraPosition);
}

XMVECTOR CCamera::ProjectionTransform(FXMVECTOR& f3Camera){
	XMFLOAT3 project{ VectorToF3(f3Camera) };
	XMFLOAT3 camera{ VectorToF3(f3Camera) };
	
	if (camera.z != 0.0f) {
		// 카메라의 시야각이 90도가 아닌 경우 투영 사각형까지의 거리를 곱한다.
		project.x = float((camera.x * m_fProjectRectDistance) /
			(m_fAspectRatio * camera.z));
		project.y = float((camera.y * m_fProjectRectDistance) /
			camera.z);
		// 투영 좌표계는 2차원이므로 z 좌표에 카메라 좌표 z좌표를 저장
		project.z = camera.z;
	}
	return XMLoadFloat3(&project);
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
	XMFLOAT3 move{ x,y,z };
	XMVECTOR v{ XMLoadFloat3(&move) };
	
	DirectX::XMStoreFloat3(&position, XMVectorAdd(XMLoadFloat3(&position), v));
}

void CCamera::Rotate(float fPitch, float fYaw, float fRoll){
	XMFLOAT3 rotation{ fPitch, fYaw, fRoll };
	XMVECTOR v{ XMLoadFloat3(&rotation) };
	DirectX::XMStoreFloat3(&rotation, XMVectorAdd(XMLoadFloat3(&rotation), v));
}
