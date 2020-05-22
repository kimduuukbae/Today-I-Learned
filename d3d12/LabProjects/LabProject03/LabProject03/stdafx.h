#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.

#include <windows.h>

#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <cmath>

#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")


#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

using namespace DirectX;
using namespace DirectX::PackedVector;

#define DIR_FORWARD 0x01
#define DIR_BACKWARD 0x02
#define DIR_LEFT 0x04
#define DIR_RIGHT 0x08
#define DIR_UP 0x10
#define DIR_DOWN 0x20

#define FRAMEBUFFER_WIDTH	640
#define FRAMEBUFFER_HEIGHT	480

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

namespace Matrix4x4 {
	using namespace DirectX;
	inline XMFLOAT4X4 identity(){
		XMFLOAT4X4 xmmtx4x4Result{};
		XMStoreFloat4x4(&xmmtx4x4Result, XMMatrixIdentity());
		return xmmtx4x4Result;
	}
}