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


#ifndef FailedAssert
#define FailedAssert(value)					\
{											\
	if (FAILED(value)) [[unlikely]] { assert(ERROR); }	\
}										
#endif

#ifndef ReleaseCom
#define ReleaseCom(x) { if(x){ x->Release(); x = 0; } }
#endif

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
