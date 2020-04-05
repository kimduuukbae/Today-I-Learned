#include "stdafx.h"
#include "Camera.h"

CPoint3D CCamera::CameraTransform(CPoint3D& f3World) {
	CPoint3D f3Camera = { f3World };

	f3Camera.x -= m_fxPosition;
	f3Camera.y -= m_fyPosition;
	f3Camera.z -= m_fzPosition;

	float fPitch = degreeToRadian(-m_fxRotation);
	float fYaw = degreeToRadian(-m_fyRotation);
	float fRoll = degreeToRadian(-m_fzRotation);

	CPoint3D f3Rotated = { f3Camera };
	if (fPitch != 0.0f) {
		f3Rotated.y = f3Camera.y * std::cosf(fPitch) - f3Camera.z * std::sinf(fPitch);
		f3Rotated.z = f3Camera.y * std::sinf(fPitch) + f3Camera.z * std::cosf(fPitch);
		f3Camera.y = f3Rotated.y;
		f3Camera.z = f3Rotated.z;
	}
	if (fYaw != 0.0f) {
		f3Rotated.x = f3Camera.x * std::cosf(fYaw) + f3Camera.z * std::sinf(fYaw);
		f3Rotated.z = -f3Camera.x * std::sinf(fYaw) + f3Camera.z * std::cosf(fYaw);
		f3Camera.x = f3Rotated.x;
		f3Camera.z = f3Rotated.z;
	}
	if (fRoll != 0.0f) {
		f3Rotated.x = f3Camera.x * std::cosf(fRoll) - f3Camera.y * std::sinf(fRoll);
		f3Rotated.y = f3Camera.x * std::sinf(fRoll) + f3Camera.y * std::cosf(fRoll);
		f3Camera.x = f3Rotated.x;
		f3Camera.y = f3Rotated.y;
	}
	return f3Camera;
}

CPoint3D CCamera::ProjectionTransform(CPoint3D& f3Camera){
	CPoint3D f3Project = f3Camera;
	if (f3Camera.z != 0.0f) {
		// ī�޶��� �þ߰��� 90���� �ƴ� ��� ���� �簢�������� �Ÿ��� ���Ѵ�.
		f3Project.x = float((f3Camera.x * m_fProjectRectDistance) /
			(m_fAspectRatio * f3Camera.z));
		f3Project.y = float((f3Camera.y * m_fProjectRectDistance) /
			f3Camera.z);
		// ���� ��ǥ��� 2�����̹Ƿ� z ��ǥ�� ī�޶� ��ǥ z��ǥ�� ����
		f3Project.z = f3Camera.z;
	}
	return f3Project;
}

CPoint3D CCamera::ScreenTransform(CPoint3D& f3Projection){
	// ���� ��ǥ���� ���� ȭ�� ��ǥ��� ��ȯ�ϴ� �Լ�
	CPoint3D f3Screen = { f3Projection };
	float fHalfWidth = { m_pViewport->m_nWidth * 0.5f };
	float fHalfHeight = { m_pViewport->m_nHeight * 0.5f };
	f3Screen.x = (f3Projection.x * fHalfWidth) + m_pViewport->m_nLeft + fHalfWidth;
	f3Screen.y = (-f3Projection.y * fHalfHeight) + m_pViewport->m_nTop + fHalfHeight;
	return f3Screen;
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
	m_fxPosition += x;
	m_fyPosition += y;
	m_fzPosition += z;

}

void CCamera::Rotate(float fPitch, float fYaw, float fRoll){
	m_fxRotation += fPitch;
	m_fyRotation += fYaw;
	m_fzRotation += fRoll;
}
