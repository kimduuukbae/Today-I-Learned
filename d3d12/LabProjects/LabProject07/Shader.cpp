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
	commandList->SetGraphicsRoot32BitConstants(0, 16, &world, 0);
}

void CShader::OnPrepareRender(ID3D12GraphicsCommandList* commandList){
	commandList->SetPipelineState(pipelineStates[0]);
}

void CShader::Render(ID3D12GraphicsCommandList* commandList, CCamera* pCamera){
	OnPrepareRender(commandList);
}

CDiffusedShader::CDiffusedShader(){
}

CDiffusedShader::~CDiffusedShader(){
}

D3D12_INPUT_LAYOUT_DESC CDiffusedShader::CreateInputLayout(){
	UINT inputElementCount{ 2 };
	D3D12_INPUT_ELEMENT_DESC* inputElementDesc{ new D3D12_INPUT_ELEMENT_DESC[inputElementCount]{} };

	inputElementDesc[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	inputElementDesc[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDesc;
	inputLayoutDesc.NumElements = inputElementCount;

	return inputLayoutDesc;
}

D3D12_SHADER_BYTECODE CDiffusedShader::CreateVertexShader(ID3DBlob** shaderBlob){
	return CompileShaderFromFile(L"Shaders.hlsl", "VSDiffused", "vs_5_1", shaderBlob);
}

void CDiffusedShader::CreateShader(ID3D12Device* device, ID3D12RootSignature* rootSignature){
	pipelineStateCount = 1;
	pipelineStates = new ID3D12PipelineState * [pipelineStateCount];
	CShader::CreateShader(device, rootSignature);
}

D3D12_SHADER_BYTECODE CDiffusedShader::CreatePixelShader(ID3DBlob** shaderBlob){
	return CompileShaderFromFile(L"Shaders.hlsl", "PSDiffused", "ps_5_1", shaderBlob);
}
