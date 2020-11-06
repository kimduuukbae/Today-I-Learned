#include "stdafx.h"
#include "Mesh.h"
#include "Common.h"
#include "GameplayStatics.h"
#include "MeshManager.h"

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
	cmdList->IASetPrimitiveTopology(primTopology);
}

void Mesh::Draw(ID3D12GraphicsCommandList* cmdList)
{
	if (iCount > 0) {
		cmdList->IASetIndexBuffer(&GetIndexBufferView());
		cmdList->DrawIndexedInstanced(iCount, 1, 0, 0, 0);
	}
	else
		cmdList->DrawInstanced(vCount, 1, 0, 0);
}

CustomMesh::CustomMesh()
{

}

Mesh* CustomMesh::CreateMesh(std::vector<Vertex>* v, 
	std::vector<uint32_t>* i,
	D3D12_PRIMITIVE_TOPOLOGY pTopology,
	const std::string& meshName)
{
	if (!v)
		return nullptr;

	auto device{ D3DApp::GetApp()->GetDevice() };
	auto commandList{ D3DApp::GetApp()->GetCommandList() };

	const UINT vbSize{ static_cast<UINT>(v->size()) * sizeof(Vertex) };

	vBuffer = Buffers::CreateDefaultBuffer(device, commandList,
		v->data(), vbSize, vUploadBuffer);

	vByteSize = vbSize;
	vByteStride = sizeof(Vertex);
	vCount = static_cast<uint32_t>(v->size());
	primTopology = pTopology;

	if (i) {
		const UINT ibSize{ static_cast<UINT>(i->size()) * sizeof(uint32_t) };
		iBuffer = Buffers::CreateDefaultBuffer(device, commandList,
			i->data(), ibSize, iUploadBuffer);

		iFormat = DXGI_FORMAT_R32_UINT;
		iByteSize = ibSize;

		iCount = static_cast<UINT>(i->size());
	}

	return this;
}

CustomMesh::~CustomMesh()
{
}
