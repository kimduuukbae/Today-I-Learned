#include "stdafx.h"
#include "Mesh.h"
#include "Common.h"

using namespace std;
using namespace Buffers;
using namespace DirectX;

MeshBase::MeshBase() { }

MeshBase::~MeshBase() { }

void MeshBase::ReleaseUploadBuffer()
{
	vUploadBuffer = nullptr;
	iUploadBuffer = nullptr;
}

D3D12_VERTEX_BUFFER_VIEW MeshBase::GetVertexBufferView()
{
	return
	{
		.BufferLocation {vBuffer->GetGPUVirtualAddress()},
		.SizeInBytes {vByteSize},
		.StrideInBytes {vByteStride}
	};
}

D3D12_INDEX_BUFFER_VIEW MeshBase::GetIndexBufferView()
{
	return
	{
		.BufferLocation {iBuffer->GetGPUVirtualAddress()},
		.SizeInBytes{iByteSize},
		.Format{iFormat}
	};
}

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}

void Mesh::BindingResource(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->IASetVertexBuffers(0, 1, &GetVertexBufferView());
	cmdList->IASetIndexBuffer(&GetIndexBufferView());
	cmdList->IASetPrimitiveTopology(primTopology);
}

void Mesh::Draw(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->DrawIndexedInstanced(iCount, 1, 0, 0, 0);
}
