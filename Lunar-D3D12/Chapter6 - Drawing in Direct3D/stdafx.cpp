#include "stdafx.h"

using namespace Microsoft::WRL;
namespace D3D {
	Microsoft::WRL::ComPtr<ID3DBlob> CompileShader(
		const std::wstring_view& fileName,
		const D3D_SHADER_MACRO* defines,
		const std::string_view& entrypoint,
		const std::string_view& target) {

		UINT compileFlags{};
#if defined(DEBUG) || defined(_DEBUG)
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		ComPtr<ID3DBlob> byteCode{ nullptr };
		ComPtr<ID3DBlob> errorBlob{ nullptr };
		HRESULT hr = D3DCompileFromFile(fileName.data(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entrypoint.data(), target.data(), compileFlags, 0, &byteCode, &errorBlob);

		if (errorBlob)
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		FailedAssert(hr);

		return byteCode;
	}

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateDefaultBuffer(
		ID3D12Device* device,
		ID3D12GraphicsCommandList* cmdList,
		const void* initData,
		UINT64 byteSize,
		Microsoft::WRL::ComPtr<ID3D12Resource>& uploadBuffer) {

		ComPtr<ID3D12Resource> defaultBuffer{ nullptr };

		D3D12_HEAP_PROPERTIES prop{};
		prop.Type = D3D12_HEAP_TYPE_DEFAULT;

		D3D12_RESOURCE_DESC desc{};
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.Alignment = 0;
		desc.Width = byteSize;
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags = D3D12_RESOURCE_FLAG_NONE;
		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;

		// 실제 디폴트 버퍼 생성
		FailedAssert(device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(defaultBuffer.GetAddressOf())));

		// CPU 메모리의 자료를 기본 버퍼에 올리려면 업로드 힙이 필요함
		// CPU는 디폴트 힙(버퍼)와 통신할 수 없음.
		prop.Type = D3D12_HEAP_TYPE_UPLOAD;
		FailedAssert(device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(uploadBuffer.GetAddressOf())));

		// 업로드힙 -> 디폴트버퍼로 자료 복사 요청
		BYTE* dataBegin{ nullptr };
		HRESULT hr = uploadBuffer.Get()->Map(0, nullptr, reinterpret_cast<void**>(&dataBegin));
		memcpy(dataBegin, initData, byteSize);
		uploadBuffer.Get()->Unmap(0, nullptr);

		cmdList->CopyResource(defaultBuffer.Get(), uploadBuffer.Get());

		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Transition.pResource = defaultBuffer.Get();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
		cmdList->ResourceBarrier(1, &barrier);

		return defaultBuffer;
	}
}