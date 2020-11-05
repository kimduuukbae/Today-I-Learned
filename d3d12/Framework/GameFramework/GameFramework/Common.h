#pragma once

namespace Math
{
	constexpr DirectX::XMFLOAT4X4 Identity4x4()
	{
		return
		{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
	}

	constexpr DirectX::XMFLOAT3 CrossProduct(DirectX::XMFLOAT3& v1, DirectX::XMFLOAT3& v2, bool normalize = true)
	{
		using namespace DirectX;
		XMFLOAT3 result;
		if (normalize)
			XMStoreFloat3(&result, XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&v1), XMLoadFloat3(&v2))));
		else
			XMStoreFloat3(&result, XMVector3Cross(XMLoadFloat3(&v1), XMLoadFloat3(&v2)));
		return result;
	}
}

namespace Buffers
{
	constexpr std::uint32_t ConstantBufferByteSize(std::uint32_t byte)
	{
		// 예) byte == 300,
		// (300 + 255) = 555 & ~255
		// 0x022B & 0xFF00
		// 0x0200 = 512

		// 상수 버퍼 원소의 크기는 반드시 256바이트의 배수여야 함.
		// 왜냐면 하드웨어는 256바이트 오프셋에서 시작하는 n * 256 바이트 길이의 
		// 상수 데이터만 볼 수 있음
		return (byte + 255) & ~255;
	}

	template <typename T>
	class UploadBuffer
	{
	public:
		UploadBuffer(ID3D12Device* device, std::uint32_t elementCount, bool isConstantBuffer)
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
			// Resource 를 다 사용하기 전에는 UnMap 호출 필요 X
			// 근데, GPU가 사용중일땐, CPU가 Resource에 접근 못하도록 해야 함.
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

		void CopyData(std::size_t elementIndex, const T& data) 
		{
			memcpy(&mappedData[elementIndex * byteSize], &data, sizeof(T));
		}

		UploadBuffer(const UploadBuffer& rhs) = delete;
		UploadBuffer& operator=(const UploadBuffer& rhs) = delete;

	private:
		bool mIsConstantBuffer{ false };
		std::uint32_t byteSize{};

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