#include "stdafx.h"
#include "Mesh.h"
#include "Common.h"

using namespace std;
using namespace Buffers;
using namespace DirectX;

MeshBase::MeshBase() { }

MeshBase::~MeshBase() { }


void MeshBase::Assign(size_t count)
{
	usedCount += count;
	if (usedCount > maxCount)
		IncreaseConstantBuffer();
}

void MeshBase::ReleaseUploadBuffer()
{
	vUploadBuffer = nullptr;
	iUploadBuffer = nullptr;
}

void MeshBase::IncreaseConstantBuffer()
{
	maxCount += (maxCount / 2);

	//cBuffer = nullptr;

	//cBuffer = make_unique<UploadBuffer<XMFLOAT4X4>>(D3DApp::GetApp(), maxCount, true);
}

Mesh::Mesh()
{
}

Mesh::~Mesh()
{
}
