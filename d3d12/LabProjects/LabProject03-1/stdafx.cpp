#include "stdafx.h"

ID3D12Resource* CreateBufferResource(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList, void* pData, UINT nBytes, D3D12_HEAP_TYPE d3dHeapType, D3D12_RESOURCE_STATES d3dResourceStates, ID3D12Resource** ppd3dUploadBuffer){
    ID3D12Resource* pd3dBuffer{ nullptr };

    D3D12_HEAP_PROPERTIES d3dHeapPropertiesDesc{};
    d3dHeapPropertiesDesc.Type = d3dHeapType;
    d3dHeapPropertiesDesc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN; // 어떻게 쓰일지 모름
    d3dHeapPropertiesDesc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN; // 있는지 모름
    d3dHeapPropertiesDesc.CreationNodeMask = 1;
    d3dHeapPropertiesDesc.VisibleNodeMask = 1;

    D3D12_RESOURCE_DESC d3dResourceDesc{};
    d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;    // 버퍼임
    d3dResourceDesc.Alignment = 0;      // 정렬
    d3dResourceDesc.Width = nBytes;     // byte 만큼 생성, 1차원 버퍼이므로 height 사용 X
    d3dResourceDesc.Height = 1;
    d3dResourceDesc.DepthOrArraySize = 1;   // 3차원 텍스쳐라면 사용
    d3dResourceDesc.MipLevels = 1;
    d3dResourceDesc.Format = DXGI_FORMAT_UNKNOWN;   // 알려진 바 없음
    d3dResourceDesc.SampleDesc.Count = 1;
    d3dResourceDesc.SampleDesc.Quality = 0;
    d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;    // 리소스가 행순서로 되어있음!
    d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    D3D12_RESOURCE_STATES d3dResInitStates{ D3D12_RESOURCE_STATE_COPY_DEST };
    if (d3dHeapType == D3D12_HEAP_TYPE_UPLOAD)
        d3dResInitStates = D3D12_RESOURCE_STATE_GENERIC_READ;
    else if (d3dHeapType == D3D12_HEAP_TYPE_READBACK)
        d3dResInitStates = D3D12_RESOURCE_STATE_COPY_DEST;

    pd3dDevice->CreateCommittedResource(&d3dHeapPropertiesDesc, D3D12_HEAP_FLAG_NONE, 
        &d3dResourceDesc, d3dResInitStates, nullptr, IID_PPV_ARGS(&pd3dBuffer));

    if (pData) {
        switch (d3dHeapType) {
            case D3D12_HEAP_TYPE_DEFAULT: {
                if (ppd3dUploadBuffer) {    // 업로드 힙 생성
                    d3dHeapPropertiesDesc.Type = D3D12_HEAP_TYPE_UPLOAD;
                    pd3dDevice->CreateCommittedResource(&d3dHeapPropertiesDesc, D3D12_HEAP_FLAG_NONE,
                        &d3dResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&ppd3dUploadBuffer));

                    // 업로드 힙을 매핑하여 초기화 데이터를 업로드 힙에 복사
                    D3D12_RANGE d3dReadRange{ 0, 0 }; // 이 리소스를 CPU에서 읽을 생각 없다.
                    UINT8* pBufferDataBegin{ nullptr };
                    (*ppd3dUploadBuffer)->Map(0, &d3dReadRange, reinterpret_cast<void**>(&pBufferDataBegin));
                    // 메모리(가상)를 할당하고 리소스의 로컬 복사본을 만듬 ( 이때 리소스
                    memcpy(pBufferDataBegin, pData, nBytes);    // 편집
                    (*ppd3dUploadBuffer)->Unmap(0, nullptr);
                    // 수정된 메모리를 리소스에 복사함!
                    // 업로드 힙 내용을 디폴트 힙에 복사한다.

                    pd3dCommandList->CopyResource(pd3dBuffer, *ppd3dUploadBuffer);

                    D3D12_RESOURCE_BARRIER d3dResourceBarrier{};

                    d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
                    d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
                    d3dResourceBarrier.Transition.pResource = pd3dBuffer;
                    d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
                    d3dResourceBarrier.Transition.StateAfter = d3dResourceStates;
                    d3dResourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
                    pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
                }
                break;
            }
            case D3D12_HEAP_TYPE_UPLOAD: {
                D3D12_RANGE d3dReadRange{ 0, 0 };
                UINT8* pBufferDataBegin{ nullptr };
                pd3dBuffer->Map(0, &d3dReadRange, (void**)&pBufferDataBegin);
                memcpy(pBufferDataBegin, pData, nBytes);
                pd3dBuffer->Unmap(0, NULL);
                break;
            }
            case D3D12_HEAP_TYPE_READBACK:
                break;
        }
    }
    return pd3dBuffer;
}
