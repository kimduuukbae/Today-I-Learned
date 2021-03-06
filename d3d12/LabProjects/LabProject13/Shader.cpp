#include "stdafx.h"
#include "Shader.h"
#include "GameObject.h"

CShader::CShader(){

}

CShader::~CShader(){
	if (pipelineStates) {
		for (int i = 0; i < pipelineStateCount; ++i)
			if (pipelineStates[i])
				pipelineStates[i]->Release();

		delete[] pipelineStates;
	}
}

D3D12_INPUT_LAYOUT_DESC CShader::CreateInputLayout() {
	D3D12_INPUT_LAYOUT_DESC inputLayout;
	inputLayout.NumElements = 0;
	inputLayout.pInputElementDescs = nullptr;
	
	return inputLayout;
}

D3D12_RASTERIZER_DESC CShader::CreateRasterizerState(){
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID; // 내부를 어떻게 채울것인가? 다 색칠
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;	 // 안보이는 면을 어떻게 할 것임?
	// 와인딩 오더-> 시계방향으로 나열 법선벡터를 구하고 카메라랑 내적을 할때 보이면 둔각, 안보이면 예각
	rasterizerDesc.FrontCounterClockwise = false;		// 전면이 시계방향? 반시계방향? false = 시계
	rasterizerDesc.DepthBias = 0;	// 픽셀의 깊이값에 더해질 깊이 바이어스 값
	rasterizerDesc.DepthBiasClamp = 0.0f;	// 픽셀의 깊이값에 더해실 최대 값
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;	// 픽셀의 기울기에 따라 바이어스 조절 값
	rasterizerDesc.DepthClipEnable = true;	// 픽셀의 깊이값(거리)를 사용한 클리핑
	rasterizerDesc.MultisampleEnable = false;	// 다중샘플링
	rasterizerDesc.AntialiasedLineEnable = false;	// 안티 앨리어싱
	rasterizerDesc.ForcedSampleCount = 0;		// UAV 렌더링에 사용할 샘플 갯수
	rasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;	
	// 보수적 레스터라이저 사용?
	// 레스터라이저가 원근 투영 나누기 / 깊이 테스트를 하니까...?
	
	return rasterizerDesc;
}

D3D12_BLEND_DESC CShader::CreateBlendState(){
	D3D12_BLEND_DESC blendDesc{};
	blendDesc.AlphaToCoverageEnable = false;
	blendDesc.IndependentBlendEnable = false;
	blendDesc.RenderTarget[0].BlendEnable = false;
	blendDesc.RenderTarget[0].LogicOpEnable = false;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return blendDesc;
}

D3D12_DEPTH_STENCIL_DESC CShader::CreateDepthStencilState(){
	D3D12_DEPTH_STENCIL_DESC dsDesc{};
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = 0;
	dsDesc.StencilWriteMask = 0;
	dsDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	dsDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return dsDesc;
}

D3D12_SHADER_BYTECODE CShader::CreateVertexShader(ID3DBlob** shaderBlob){
	D3D12_SHADER_BYTECODE bytecode{};
	bytecode.BytecodeLength = 0;
	bytecode.pShaderBytecode = nullptr;

	return bytecode;
}

D3D12_SHADER_BYTECODE CShader::CreatePixelShader(ID3DBlob** shaderBlob){
	D3D12_SHADER_BYTECODE bytecode{};
	bytecode.BytecodeLength = 0;
	bytecode.pShaderBytecode = nullptr;

	return bytecode;
}

D3D12_SHADER_BYTECODE CShader::CompileShaderFromFile(const WCHAR* fileName, LPCSTR shaderName, LPCSTR shaderProfile, ID3DBlob** shaderBlob){
	UINT nCompileFlags{};
#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
	::D3DCompileFromFile(fileName, nullptr, nullptr, shaderName, shaderProfile, nCompileFlags, 0, shaderBlob, nullptr);

	D3D12_SHADER_BYTECODE shaderBytecode{};
	shaderBytecode.BytecodeLength = (*shaderBlob)->GetBufferSize();
	shaderBytecode.pShaderBytecode = (*shaderBlob)->GetBufferPointer();
	return shaderBytecode;
}

void CShader::CreateShader(ID3D12Device* device, ID3D12RootSignature* rootSignature){

	ID3DBlob* vsBlob{ nullptr }, * psBlob{ nullptr };

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc{};
	pipelineStateDesc.pRootSignature = rootSignature;
	pipelineStateDesc.VS = CreateVertexShader(&vsBlob);
	pipelineStateDesc.PS = CreatePixelShader(&psBlob);
	pipelineStateDesc.RasterizerState = CreateRasterizerState();
	pipelineStateDesc.BlendState = CreateBlendState();
	pipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	pipelineStateDesc.InputLayout = CreateInputLayout();
	pipelineStateDesc.SampleMask = UINT_MAX;
	pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateDesc.NumRenderTargets = 1;
	pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	pipelineStateDesc.SampleDesc.Count = 1;
	
	device->CreateGraphicsPipelineState(&pipelineStateDesc, IID_PPV_ARGS(&pipelineStates[0]));
	
	if (vsBlob) vsBlob->Release();
	if (psBlob) psBlob->Release();

	if (pipelineStateDesc.InputLayout.pInputElementDescs) delete[] pipelineStateDesc.InputLayout.pInputElementDescs;

}

void CShader::CreateShaderVariables(ID3D12Device* device, ID3D12CommandList* commandList){

}

void CShader::ReleaseShaderVariables() {

}

void CShader::UpdateShaderVariables(ID3D12GraphicsCommandList* commandList, XMFLOAT4X4* worldMatrix){
	XMFLOAT4X4 world{};
	XMStoreFloat4x4(&world, XMMatrixTranspose(XMLoadFloat4x4(worldMatrix)));
	//commandList->SetGraphicsRoot32BitConstants(0, 16, &world, 0);
}

void CShader::OnPrepareRender(ID3D12GraphicsCommandList* commandList){
	commandList->SetPipelineState(pipelineStates[0]);
}

void CShader::Render(ID3D12GraphicsCommandList* commandList, CCamera* pCamera){
	OnPrepareRender(commandList);
}

CPlayerShader::CPlayerShader(){
}

CPlayerShader::~CPlayerShader(){
}

D3D12_INPUT_LAYOUT_DESC CPlayerShader::CreateInputLayout(){
	UINT inputElementCount{ 2 };
	D3D12_INPUT_ELEMENT_DESC* inputElementDesc{ new D3D12_INPUT_ELEMENT_DESC[inputElementCount]{} };

	inputElementDesc[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	inputElementDesc[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDesc;
	inputLayoutDesc.NumElements = inputElementCount;

	return inputLayoutDesc;
}

D3D12_SHADER_BYTECODE CPlayerShader::CreateVertexShader(ID3DBlob** shaderBlob){
	return CompileShaderFromFile(L"Shaders.hlsl", "VSDiffused", "vs_5_1", shaderBlob);
}

void CPlayerShader::CreateShader(ID3D12Device* device, ID3D12RootSignature* rootSignature){
	pipelineStateCount = 1;
	pipelineStates = new ID3D12PipelineState * [pipelineStateCount];
	CShader::CreateShader(device, rootSignature);
}

D3D12_SHADER_BYTECODE CPlayerShader::CreatePixelShader(ID3DBlob** shaderBlob){
	return CompileShaderFromFile(L"Shaders.hlsl", "PSDiffused", "ps_5_1", shaderBlob);
}

CObjectsShader::CObjectsShader(){

}

CObjectsShader::~CObjectsShader(){

}

void CObjectsShader::BuildObjects(ID3D12Device* device, ID3D12GraphicsCommandList* commandList){
	/*CCubeMeshDiffused* pCubeMesh = new CCubeMeshDiffused(device, commandList, 12.0f, 12.0f, 12.0f);
	int xObjects{ 10 }, yObjects{ 10 }, zObjects{ 10 }, i{ 0 };
	m_nObjects = (xObjects * 2 + 1) * (yObjects * 2 + 1) * (zObjects * 2 + 1);
	m_ppObjects = new CGameObject * [m_nObjects];
	float fxPitch = 12.0f * 2.5f;
	float fyPitch = 12.0f * 2.5f;
	float fzPitch = 12.0f * 2.5f;
	CRotatingObject* pRotatingObject = NULL;
	for (int x = -xObjects; x <= xObjects; x++)
	{
		for (int y = -yObjects; y <= yObjects; y++)
		{
			for (int z = -zObjects; z <= zObjects; z++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetMesh(pCubeMesh);
				//각 정육면체 객체의 위치를 설정한다.
				pRotatingObject->SetPosition(fxPitch*x, fyPitch*y, fzPitch*z);
				pRotatingObject->SetRotatinAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
				pRotatingObject->SetRotationSpeed(10.0f * (i % 10) + 3.0f);
				m_ppObjects[i++] = pRotatingObject;
			}
		}
	}
	CreateShaderVariables(device, commandList);*/
}

void CObjectsShader::AnimateObjects(float elapsedTime){
	for (int j = 0; j < m_nObjects; j++)
		m_ppObjects[j]->Animate(elapsedTime);
	
}

void CObjectsShader::ReleaseObjects(){
	{
		if (m_ppObjects){}
			for (int j = 0; j < m_nObjects; j++)
				if (m_ppObjects[j]) delete m_ppObjects[j];
			delete[] m_ppObjects;
		}
}

D3D12_INPUT_LAYOUT_DESC CObjectsShader::CreateInputLayout(){
	UINT inputElementCount{ 2 };
	D3D12_INPUT_ELEMENT_DESC* inputElementDesc{ new D3D12_INPUT_ELEMENT_DESC[inputElementCount]{} };

	inputElementDesc[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	inputElementDesc[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDesc;
	inputLayoutDesc.NumElements = inputElementCount;

	return inputLayoutDesc;
}

D3D12_SHADER_BYTECODE CObjectsShader::CreateVertexShader(ID3DBlob** shaderBlob){
	return CompileShaderFromFile(L"Shaders.hlsl", "VSDiffused", "vs_5_1", shaderBlob);
}

D3D12_SHADER_BYTECODE CObjectsShader::CreatePixelShader(ID3DBlob** shaderBlob){
	return CompileShaderFromFile(L"Shaders.hlsl", "PSDiffused", "ps_5_1", shaderBlob);
}

void CObjectsShader::CreateShader(ID3D12Device* device, ID3D12RootSignature* rootSignature){
	pipelineStateCount = 1;
	pipelineStates = new ID3D12PipelineState * [pipelineStateCount];

	CShader::CreateShader(device, rootSignature);
}

void CObjectsShader::ReleaseUploadBuffers(){
	if (m_ppObjects)
		for (int i = 0; i < m_nObjects; ++i) m_ppObjects[i]->ReleaseUploadBuffers();
}

void CObjectsShader::Render(ID3D12GraphicsCommandList* commandList, CCamera* pCamera){
	CShader::Render(commandList, pCamera);
//	for (int i = 0; i < m_nObjects; ++i)
//		if (m_ppObjects[i])
//			m_ppObjects[i]->Render(commandList, pCamera);
}

CInstancingShader::CInstancingShader() {

}

CInstancingShader::~CInstancingShader() {

}

D3D12_INPUT_LAYOUT_DESC CInstancingShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC* pd3dInputElementDescs = new
		D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	//정점 정보를 위한 입력 원소이다. 
	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;
	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CInstancingShader::CreateVertexShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSInstancing", "vs_5_1",
		ppd3dShaderBlob));
}
D3D12_SHADER_BYTECODE CInstancingShader::CreatePixelShader(ID3DBlob** ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSInstancing", "ps_5_1",
		ppd3dShaderBlob));
}

void CInstancingShader::CreateShader(ID3D12Device* pd3dDevice, ID3D12RootSignature
	* pd3dGraphicsRootSignature){
	pipelineStateCount = 1;
	pipelineStates = new ID3D12PipelineState * [pipelineStateCount];
	CShader::CreateShader(pd3dDevice, pd3dGraphicsRootSignature);
}

void CInstancingShader::CreateShaderVariables(ID3D12Device* pd3dDevice,
	ID3D12GraphicsCommandList* pd3dCommandList){
	//인스턴스 정보를 저장할 정점 버퍼를 업로드 힙 유형으로 생성한다.
	m_pd3dcbGameObjects = ::CreateBufferResource(pd3dDevice, pd3dCommandList, NULL,
	sizeof(VS_VB_INSTANCE)* m_nObjects, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_GENERIC_READ, NULL);
		//정점 버퍼(업로드 힙)에 대한 포인터를 저장한다. 
	m_pd3dcbGameObjects->Map(0, NULL, (void **)&m_pcbMappedGameObjects);
}

void CInstancingShader::UpdateShaderVariables(ID3D12GraphicsCommandList
	* pd3dCommandList){
	pd3dCommandList->SetGraphicsRootShaderResourceView(2,
		m_pd3dcbGameObjects->GetGPUVirtualAddress());
	for (int j = 0; j < m_nObjects; j++)
	{
		m_pcbMappedGameObjects[j].color = (j % 2) ? XMFLOAT4(0.5f, 0.0f, 0.0f, 0.0f) :
			XMFLOAT4(0.0f, 0.0f, 0.5f, 0.0f);
		XMStoreFloat4x4(&m_pcbMappedGameObjects[j].transform,
			XMMatrixTranspose(XMLoadFloat4x4(&m_ppObjects[j]->worldMatrix)));
	}
}

void CInstancingShader::BuildObjects(ID3D12Device* pd3dDevice, ID3D12GraphicsCommandList
	* pd3dCommandList){
	int xObjects = 10, yObjects = 10, zObjects = 10, i = 0;
	m_nObjects = (xObjects * 2 + 1) * (yObjects * 2 + 1) * (zObjects * 2 + 1);
	m_ppObjects = new CGameObject * [m_nObjects];
	float fxPitch = 12.0f * 2.5f;
	float fyPitch = 12.0f * 2.5f;
	float fzPitch = 12.0f * 2.5f;
	CRotatingObject* pRotatingObject = NULL;
	for (int x = -xObjects; x <= xObjects; x++)
	{
		for (int y = -yObjects; y <= yObjects; y++)
		{
			for (int z = -zObjects; z <= zObjects; z++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->SetPosition(fxPitch * x, fyPitch * y, fzPitch * z);
				pRotatingObject->SetRotatinAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
				pRotatingObject->SetRotationSpeed(10.0f * (i % 10));
				m_ppObjects[i++] = pRotatingObject;
			}
		}
	}
	//인스턴싱을 사용하여 렌더링하기 위하여 하나의 게임 객체만 메쉬를 가진다. 
	CCubeMeshDiffused *pCubeMesh = new CCubeMeshDiffused(pd3dDevice, pd3dCommandList,
	12.0f, 12.0f, 12.0f);
	m_ppObjects[0]->SetMesh(pCubeMesh);
	//인스턴싱을 위한 버퍼(Structured Buffer)를 생성한다. 
	CreateShaderVariables(pd3dDevice, pd3dCommandList);
}

void CInstancingShader::ReleaseShaderVariables() {
	if (m_pd3dcbGameObjects) m_pd3dcbGameObjects->Unmap(0, NULL);
	if (m_pd3dcbGameObjects) m_pd3dcbGameObjects->Release();
}

void CInstancingShader::Render(ID3D12GraphicsCommandList* pd3dCommandList, CCamera
	* pCamera){
	CObjectsShader::Render(pd3dCommandList, pCamera);
	//모든 게임 객체의 인스턴싱 데이터를 버퍼에 저장한다.
	UpdateShaderVariables(pd3dCommandList);
	//하나의 정점 데이터를 사용하여 모든 게임 객체(인스턴스)들을 렌더링한다. 
	m_ppObjects[0]->Render(pd3dCommandList, pCamera, m_nObjects);
}