#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

#include <windows.h>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <cmath>

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

#define FRAMEBUFFER_WIDTH	640
#define FRAMEBUFFER_HEIGHT	480
#define degreeToRadian(x)	float((x)*3.141592654f / 180.0f)

[[nodiscard]]
inline DirectX::XMVECTOR F3ToVector(const DirectX::XMFLOAT3& arg) {
	return DirectX::XMLoadFloat3(&arg);
}

[[nodiscard]]
inline DirectX::XMFLOAT3 VectorToF3(const DirectX::XMVECTOR& arg) {
	DirectX::XMFLOAT3 nrvo{};
	DirectX::XMStoreFloat3(&nrvo, arg);
	return nrvo;
}