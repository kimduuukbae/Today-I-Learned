#pragma once

#include "Defines.h"

namespace Buffers
{
	constexpr std::uint64_t ConstantBufferByteSize(std::uint64_t byte)
	{
		// ��) byte == 300,
		// (300 + 255) = 555 & ~255
		// 0x022B & 0xFF00
		// 0x0200 = 512

		// ��� ���� ������ ũ��� �ݵ�� 256����Ʈ�� ������� ��.
		// �ֳĸ� �ϵ����� 256����Ʈ �����¿��� �����ϴ� n * 256 ����Ʈ ������ 
		// ��� �����͸� �� �� ����
		return (byte + 255) & ~255;
	}

	template <typename T>
	class UploadBuffer
	{
	public:
		UploadBuffer(ID3D12Device* device, std::uint64_t elementCount, bool isConstantBuffer)
			: mIsConstantBuffer{ isConstantBuffer }{
			if (isConstantBuffer)
				byteSize = ConstantBufferByteSize(sizeof(T));

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

		void CopyData(const T& data) 
		{
			memcpy(mappedData, &data, sizeof(T));
		}

		UploadBuffer(const UploadBuffer& rhs) = delete;
		UploadBuffer& operator=(const UploadBuffer& rhs) = delete;

	private:
		bool mIsConstantBuffer{ false };
		std::uint64_t byteSize{};

		Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer{};
		BYTE* mappedData{ nullptr };
	};

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* cmdList,
		const void* initData,
		UINT64 byteSize,
		Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer);
}

namespace Common
{
	template <typename T>
	concept is_descriptor = (std::is_same_v<T, D3D12_GPU_DESCRIPTOR_HANDLE> || std::is_same_v<T, D3D12_CPU_DESCRIPTOR_HANDLE>)
		&& requires(T & descriptor)
	{
		{ descriptor.ptr };
	};

	template <typename T>
	requires is_descriptor<T>
		T GetDescriptorHandleOffset(const T& handle, std::size_t offset, std::size_t size)
	{
		return { .ptr {handle.ptr + size * offset} };
	}
}