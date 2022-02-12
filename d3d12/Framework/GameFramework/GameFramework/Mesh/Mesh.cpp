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
}

void MeshBase::BindingResource(ID3D12GraphicsCommandList* cmdList)
{
	D3D12_VERTEX_BUFFER_VIEW v{ GetVertexBufferView() };
	cmdList->IASetVertexBuffers(0, 1, &v);
	cmdList->IASetPrimitiveTopology(primTopology);
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

Mesh::Mesh() { }
Mesh::~Mesh() { }

void Mesh::BindingResource(ID3D12GraphicsCommandList* cmdList)
{
	Super::BindingResource(cmdList);
	D3D12_INDEX_BUFFER_VIEW i{ GetIndexBufferView() };
	cmdList->IASetIndexBuffer(&i);
}

void Mesh::Draw(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->DrawIndexedInstanced(iCount, 1, 0, 0, 0);
}

void Mesh::ReleaseUploadBuffer()
{
	Super::ReleaseUploadBuffer();
	iUploadBuffer = nullptr;
}

D3D12_INDEX_BUFFER_VIEW Mesh::GetIndexBufferView()
{
	return
	{
		.BufferLocation {iBuffer->GetGPUVirtualAddress()},
		.SizeInBytes{iByteSize},
		.Format{iFormat}
	};
}

CustomVertexMesh::CustomVertexMesh() {}
CustomVertexMesh::~CustomVertexMesh() {}

void CustomVertexMesh::BindingResource(ID3D12GraphicsCommandList* cmdList)
{
	Super::BindingResource(cmdList);
}

void CustomVertexMesh::ReleaseUploadBuffer()
{
	Super::ReleaseUploadBuffer();
}

void CustomVertexMesh::Draw(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->DrawInstanced(vCount, 1, 0, 0);
}

MeshBase* CustomVertexMesh::CreateMesh(std::vector<Vertex>* v,
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

	return this;
}

CustomIndexMesh::CustomIndexMesh(){}
CustomIndexMesh::~CustomIndexMesh(){}

void CustomIndexMesh::BindingResource(ID3D12GraphicsCommandList* cmdList)
{
	Super::BindingResource(cmdList);
	D3D12_INDEX_BUFFER_VIEW i{ GetIndexBufferView() };
	cmdList->IASetIndexBuffer(&i);
}

void CustomIndexMesh::ReleaseUploadBuffer()
{
	Super::ReleaseUploadBuffer();
	iUploadBuffer = nullptr;
}

void CustomIndexMesh::Draw(ID3D12GraphicsCommandList* cmdList)
{
	cmdList->DrawIndexedInstanced(iCount, 1, 0, 0, 0);
}

MeshBase* CustomIndexMesh::CreateMesh(std::vector<Vertex>* v, std::vector<uint32_t>* i, D3D12_PRIMITIVE_TOPOLOGY pTopology, const std::string& meshName)
{
	Super::CreateMesh(v, pTopology, meshName);

	auto device{ D3DApp::GetApp()->GetDevice() };
	auto commandList{ D3DApp::GetApp()->GetCommandList() };

	const UINT ibSize{ static_cast<UINT>(i->size()) * sizeof(std::uint32_t) };

	iBuffer = Buffers::CreateDefaultBuffer(device, commandList,
		i->data(), ibSize, iUploadBuffer);

	iByteSize = ibSize;
	iCount = static_cast<uint32_t>(i->size());

	return this;
}

D3D12_INDEX_BUFFER_VIEW CustomIndexMesh::GetIndexBufferView()
{
	return
	{
		.BufferLocation {iBuffer->GetGPUVirtualAddress()},
		.SizeInBytes{iByteSize},
		.Format{iFormat}
	};
}

FrameMesh::FrameMesh() { }

FrameMesh::~FrameMesh() { }

void FrameMesh::ReleaseUploadBuffer()
{
	Super::ReleaseUploadBuffer();
	for (auto& it : subIUploadBuffers)
		it = nullptr;
}

void FrameMesh::BindingResource(ID3D12GraphicsCommandList* cmdList)
{
	Super::BindingResource(cmdList);
}

void FrameMesh::Draw(ID3D12GraphicsCommandList* cmdList)
{
	for (int i = 0; i < subIBuffers.size(); ++i) {

		D3D12_INDEX_BUFFER_VIEW ibView{ GetIndexBufferView(i) };

		cmdList->IASetIndexBuffer(&ibView);
		cmdList->DrawIndexedInstanced(subIBuffers[i].iCount, 1, 0, 0, 0);
	}
}

void FrameMesh::SetVertex(const std::vector<Vertex>& vertices)
{
	auto device{ D3DApp::GetApp()->GetDevice() };
	auto commandList{ D3DApp::GetApp()->GetCommandList() };

	const UINT vbSize{ static_cast<UINT>(vertices.size()) * sizeof(Vertex) };

	vBuffer = Buffers::CreateDefaultBuffer(device, commandList,
		vertices.data(), vbSize, vUploadBuffer);

	vByteSize = vbSize;
	vByteStride = sizeof(Vertex);
	vCount = static_cast<uint32_t>(vertices.size());
}

void FrameMesh::AddSubMesh(const std::vector<uint32_t>& indices)
{
	auto device{ D3DApp::GetApp()->GetDevice() };
	auto commandList{ D3DApp::GetApp()->GetCommandList() };

	const UINT ibSize{ static_cast<UINT>(indices.size()) * sizeof(uint32_t) };

	subIBuffers.emplace_back();
	subIUploadBuffers.emplace_back();

	subIBuffers.back().subIBuffer = Buffers::CreateDefaultBuffer(device, commandList,
		indices.data(), ibSize, subIUploadBuffers.back());

	subIBuffers.back().iByteSize = ibSize;
	subIBuffers.back().iCount = static_cast<uint32_t>(indices.size());
}

D3D12_INDEX_BUFFER_VIEW FrameMesh::GetIndexBufferView(size_t where)
{
	return
	{
		.BufferLocation {subIBuffers[where].subIBuffer->GetGPUVirtualAddress()},
		.SizeInBytes{subIBuffers[where].iByteSize},
		.Format{iFormat}
	};
}
