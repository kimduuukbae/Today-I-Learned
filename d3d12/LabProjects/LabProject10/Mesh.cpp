#include "stdafx.h"
#include "Mesh.h"

CMesh::CMesh(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList* pd3dCommandList){

}
CMesh::~CMesh() {
	if (vertexBuffer) vertexBuffer->Release();
	if (vertexUploadBuffer) vertexUploadBuffer->Release();
	if (indexBuffer) indexBuffer->Release();
	if (indexUploadBuffer) indexUploadBuffer->Release();
}

void CMesh::ReleaseUploadBuffers(){
	if (indexUploadBuffer) indexUploadBuffer->Release();
	if (vertexUploadBuffer) vertexUploadBuffer->Release();
	indexUploadBuffer = nullptr;
	vertexUploadBuffer = nullptr;
}

void CMesh::Render(ID3D12GraphicsCommandList* commandList){
	commandList->IASetPrimitiveTopology(primTopology);
	commandList->IASetVertexBuffers(slot, 1, &vertexBufferView);	// 버텍스 버퍼 뷰 설정
	if (indexBuffer) {
		commandList->IASetIndexBuffer(&indexBufferView);
		commandList->DrawIndexedInstanced(indices, 1, 0, baseVertex, startIndex);
	}
	else
		commandList->DrawInstanced(vertices, 1, offset, 0);
}

CTriangleMesh::CTriangleMesh(ID3D12Device* device, ID3D12GraphicsCommandList* commandList)
	:CMesh(device, commandList){
	vertices = 3;
	stride = sizeof(CDiffusedVertex);
	primTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	// 시계방향 순서대로 빨간색, 녹색, 파란색으로 지정
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

CCubeMeshDiffused::CCubeMeshDiffused(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, float fWidth, float fHeight, float fDepth)
: CMesh(device, commandList) {

	vertices = 8;
	stride = sizeof(CDiffusedVertex);
	primTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;

	CDiffusedVertex pVertices[8];
	pVertices[0] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	pVertices[1] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	pVertices[2] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	pVertices[3] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	pVertices[4] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	pVertices[5] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);
	pVertices[6] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	pVertices[7] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);

	vertexBuffer = ::CreateBufferResource(device, commandList, pVertices,
		stride * vertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &vertexUploadBuffer);
	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.StrideInBytes = stride;
	vertexBufferView.SizeInBytes = stride * vertices;

	indices = 36;
	UINT pnIndices[36];
	//ⓐ 앞면(Front) 사각형의 위쪽 삼각형
	pnIndices[0] = 3; pnIndices[1] = 1; pnIndices[2] = 0;
	//ⓑ 앞면(Front) 사각형의 아래쪽 삼각형
	pnIndices[3] = 2; pnIndices[4] = 1; pnIndices[5] = 3;
	//ⓒ 윗면(Top) 사각형의 위쪽 삼각형
	pnIndices[6] = 0; pnIndices[7] = 5; pnIndices[8] = 4;
	//ⓓ 윗면(Top) 사각형의 아래쪽 삼각형
	pnIndices[9] = 1; pnIndices[10] = 5; pnIndices[11] = 0;
	//ⓔ 뒷면(Back) 사각형의 위쪽 삼각형
	pnIndices[12] = 3; pnIndices[13] = 4; pnIndices[14] = 7;
	//ⓕ 뒷면(Back) 사각형의 아래쪽 삼각형
	pnIndices[15] = 0; pnIndices[16] = 4; pnIndices[17] = 3;
	//ⓖ 아래면(Bottom) 사각형의 위쪽 삼각형
	pnIndices[18] = 1; pnIndices[19] = 6; pnIndices[20] = 5;
	//ⓗ 아래면(Bottom) 사각형의 아래쪽 삼각형
	pnIndices[21] = 2; pnIndices[22] = 6; pnIndices[23] = 1;
	//ⓘ 옆면(Left) 사각형의 위쪽 삼각형
	pnIndices[24] = 2; pnIndices[25] = 7; pnIndices[26] = 6;
	//ⓙ 옆면(Left) 사각형의 아래쪽 삼각형
	pnIndices[27] = 3; pnIndices[28] = 7; pnIndices[29] = 2;
	//ⓚ 옆면(Right) 사각형의 위쪽 삼각형
	pnIndices[30] = 6; pnIndices[31] = 4; pnIndices[32] = 5;
	//ⓛ 옆면(Right) 사각형의 아래쪽 삼각형
	pnIndices[33] = 7; pnIndices[34] = 4; pnIndices[35] = 6;

	indexBuffer = CreateBufferResource(device, commandList, pnIndices, sizeof(UINT) * indices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &indexUploadBuffer);

	indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	indexBufferView.SizeInBytes = sizeof(UINT) * indices;

	/*
	vertices = 36; // 면 6개 * 삼각형2개 * 버텍스 3개
	stride = sizeof(CDiffusedVertex);	// 몇 사이즈로 나누어져있음?
	offset = 0;
	slot = 0;
	primTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;
	CDiffusedVertex pVertices[36];
	int i = 0;
	//정점 버퍼 데이터는 삼각형 리스트이므로 36개의 정점 데이터를 준비한다. //ⓐ 앞면(Front) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);
	//ⓑ 앞면(Front) 사각형의 아래쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	//ⓒ 윗면(Top) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	//ⓓ 윗면(Top) 사각형의 아래쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
	//ⓔ 뒷면(Back) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	//ⓕ 뒷면(Back) 사각형의 아래쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	//ⓖ 아래면(Bottom) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	//ⓗ 아래면(Bottom) 사각형의 아래쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);
	//ⓘ 옆면(Left) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	//ⓙ 옆면(Left) 사각형의 아래쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(-fx, -fy, +fz), RANDOM_COLOR);
	//ⓚ 옆면(Right) 사각형의 위쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	//ⓛ 옆면(Right) 사각형의 아래쪽 삼각형
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, +fy, -fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, +fz), RANDOM_COLOR);
	pVertices[i++] = CDiffusedVertex(XMFLOAT3(+fx, -fy, -fz), RANDOM_COLOR);

	vertexBuffer = CreateBufferResource(device, commandList, pVertices, stride * vertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &vertexUploadBuffer);

	// 정점 버퍼 뷰 생성

	vertexBufferView.BufferLocation = vertexBuffer->GetGPUVirtualAddress();
	vertexBufferView.StrideInBytes = stride;
	vertexBufferView.SizeInBytes = stride * vertices;
	*/
}

CCubeMeshDiffused::~CCubeMeshDiffused(){

}
