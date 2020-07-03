#include "stdafx.h"
#include "Mesh.h"

CMesh::CMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList){

}
CMesh::~CMesh() {
	if (vertexBuffer) vertexBuffer->Release();
	if (vertexUploadBuffer) vertexUploadBuffer->Release();
}

void CMesh::ReleaseUploadBuffers(){
	if (vertexUploadBuffer) vertexUploadBuffer->Release();
	vertexUploadBuffer = nullptr;
}

void CMesh::Render(ID3D12GraphicsCommandList* commandList){
	commandList->IASetPrimitiveTopology(primTopology);
	commandList->IASetVertexBuffers(slot, 1, &vertexBufferView);	// ���ؽ� ���� �� ����
	commandList->DrawInstanced(vertices, 1, offset, 0);
}

CTriangleMesh::CTriangleMesh(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
	:CMesh(device, commandList){
	vertices = 3;
	stride = sizeof(CDiffusedVertex);
	primTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	// �ð���� ������� ������, ���, �Ķ������� ����
	CDiffusedVertex pVertices[3]{};
	pVertices[0] = CDiffusedVertex(XMFLOAT3{ 0.0f, 0.5f, 0.0f }, XMFLOAT4{ 1.0f, 0.0f, 0.0f, 1.0f });
	pVertices[1] = CDiffusedVertex(XMFLOAT3{ 0.5f, -0.5f, 0.0f }, XMFLOAT4{ 0.0f, 1.0f, 0.0f, 1.0f });
	pVertices[2] = CDiffusedVertex(XMFLOAT3{ -0.5f, -0.5f, 0.0f }, XMFLOAT4{ 0.0f, 0.0f, 1.0f, 1.0f });

	vertexBuffer = CreateBufferResource(device, commandList, pVertices, stride * vertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &vertexUploadBuffer);

	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.StrideInBytes = stride;
	vertexBufferView.SizeInBytes = stride * vertices;
}
