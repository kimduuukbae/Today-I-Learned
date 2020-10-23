#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN 

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
#include <array>
#include <algorithm>
#include <fstream>
#include <tuple>

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

namespace VectorMath {
	constexpr DirectX::XMFLOAT4X4 Identity4X4() {
		DirectX::XMFLOAT4X4 i{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f };

		return i;
	}
}

namespace D3D {
	constexpr std::uint32_t ConstantBufferByteSize(std::uint32_t byte)  {
		// ��) byte == 300,
		// (300 + 255) = 555 & ~255
		// 0x022B & 0xFF00
		// 0x0200 = 512
		return (byte + 255) & ~ 255;
	}

	struct Material {	// ���� ��ü�� ǥ��� ��ȣ�ۿ��ϴ� ���.
		std::string Name{};

		std::size_t MatCBIndex{ }; // �� Material �� �ش��ϴ� ��� ������ Index
		std::size_t DiffuseSrvHeapIndex{ };	//	�л� �ؽ����� Index

		DirectX::XMFLOAT4 DiffuseAlbedo{ 1.0f, 1.0f, 1.0f, 1.0f };	
		DirectX::XMFLOAT3 FresnelR0{ 0.01f, 0.01f, 0.01f };	
		float Roughness{ 0.25f };
		DirectX::XMFLOAT4X4 MatTransform{ VectorMath::Identity4X4() };
	};

	template <typename T>
	class UploadBuffer {
	public:
		// ��� ���� ������ ũ��� �ݵ�� 256����Ʈ�� ������� ��.
		// �ֳĸ� �ϵ����� 256����Ʈ �����¿��� �����ϴ� n * 256 ����Ʈ ������ 
		// ��� �����͸� �� �� ����
		UploadBuffer(ID3D12Device* device, std::uint32_t elementCount, bool isConstantBuffer)
			: mIsConstantBuffer{ isConstantBuffer }, numElement{ elementCount }{
			if (isConstantBuffer)
				byteSize = D3D::ConstantBufferByteSize(sizeof(T));

			D3D12_HEAP_PROPERTIES prop{};
			prop.Type = D3D12_HEAP_TYPE_UPLOAD;

			D3D12_RESOURCE_DESC desc{};
			desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
			desc.Width = elementCount * byteSize;
			desc.Height = 1;
			desc.DepthOrArraySize = 1;
			desc.MipLevels = 1;
			desc.Format = DXGI_FORMAT_UNKNOWN;
			desc.SampleDesc.Count = 1;
			desc.SampleDesc.Quality = 0;
			desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
			desc.Flags = D3D12_RESOURCE_FLAG_NONE;

			FailedAssert(device->CreateCommittedResource(
				&prop,
				D3D12_HEAP_FLAG_NONE,
				&desc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(&uploadBuffer)));

			FailedAssert(uploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedData)));
			// Resource �� �� ����ϱ� ������ UnMap ȣ�� �ʿ� X
			// �ٵ�, GPU�� ������϶�, CPU�� Resource�� ���� ���ϵ��� �ؾ� ��.
		}
		~UploadBuffer() {
			if (uploadBuffer) {
				uploadBuffer->Unmap(0, nullptr);
				uploadBuffer = nullptr;
			}
		}

		ID3D12Resource* GetResource() const {
			return uploadBuffer.Get();
		}

		void CopyData(std::size_t elementIndex, const T& data) {
			memcpy(&mappedData[elementIndex * byteSize], &data, sizeof(T));
		}

		std::uint32_t getElementNum() {
			return numElement;
		}

		UploadBuffer(const UploadBuffer& rhs) = delete;
		UploadBuffer& operator=(const UploadBuffer& rhs) = delete;

	private:
		bool mIsConstantBuffer{ false };
		std::uint32_t byteSize{};
		std::uint32_t numElement{};

		Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer{};
		BYTE* mappedData{ nullptr };
	};

	Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
		const std::wstring_view& fileName,
		const D3D_SHADER_MACRO* defines,
		const std::string_view& entrypoint,
		const std::string_view& target);

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* cmdList,
		const void* initData,
		UINT64 byteSize,
		Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);

	template <typename T>
	concept is_descriptor = (std::is_same_v<T, D3D12_GPU_DESCRIPTOR_HANDLE> || std::is_same_v<T, D3D12_CPU_DESCRIPTOR_HANDLE>)
		&& requires(T & descriptor)
	{
		{ descriptor.ptr };
	};

	template <typename T>
	requires is_descriptor<T>
	T GetDescriptorHandleOffset(const T& handle, std::size_t offset, std::size_t size) {
		return { .ptr {handle.ptr + size * offset} };
	}
}

