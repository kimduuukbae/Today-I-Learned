#include "stdafx.h"
#include "Camera.h"

CCamera::CCamera() {
	viewMatrix = Matrix4x4::Identity();
	projectMatrix = Matrix4x4::Identity();
	viewport = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
	scissorRect = { 0, 0, FRAME_BUFFER_WIDTH, FRAME_BUFFER_HEIGHT };
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

void CCamera::GenerateViewMatrix(XMFLOAT3 pos, XMFLOAT3 look, XMFLOAT3 up){
	viewMatrix = Matrix4x4::LookAtLH(pos, look, up);
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
