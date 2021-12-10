#pragma once

#include <windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

//simd
#if defined(_MSC_VER)
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))	// GCC , CLANG, ICC를 쓰고 x86-64, x86
#include <x86intrin.h>
#elif defined(__GNUC__) && defined(__ARM_NEON__)  // GCC , CLANG, ARMCC NEON을 포함한 ARM
#include <arm_neon.h>
#elif defined(__GNUC__) && defined(__IWMMXT__)  // GCC , CLANG, ARMCC WMMX를 포함한 ARM
#include <mmintrin.h>
#elif (defined(__GNUC__) || defined(__xlC__)) && (defined(__VEC__) || defined(__ALTIVEC__))  // GCC, CLANG, XLCC VMX 를 포함한 PowerPC
#include <altivec.h>
#elif defined(__GNUC__) && defined(__SPE__)  // GCC, CLAGN SPE를 포함한 PowerPC
#include <spe.h>
#endif

//directX
#include <d3d12.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <wrl.h>

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>
#include <dxgidebug.h>

//cpp standard
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <array>
#include <algorithm>
#include <fstream>
#include <tuple>
#include <type_traits>
#include <filesystem>
#include <functional>
#include <map>

//custom
#include "GameTimer.h"
#include "D3DApp.h"
#include "CommonMath.h"
#include "Object.h"
#include "Defines.h"

/*namespace D3D {

	struct Material {	// 빛이 물체의 표면과 상호작용하는 방식.
		std::string Name{};

		std::size_t MatCBIndex{ }; // 이 Material 에 해당하는 상수 버퍼의 Index
		std::size_t DiffuseSrvHeapIndex{ };	//	분산 텍스쳐의 Index

		DirectX::XMFLOAT4 DiffuseAlbedo{ 1.0f, 1.0f, 1.0f, 1.0f };	
		DirectX::XMFLOAT3 FresnelR0{ 0.01f, 0.01f, 0.01f };	
		float Roughness{ 0.25f };
		DirectX::XMFLOAT4X4 MatTransform{ VectorMath::Identity4X4() };
	};
}
*/
