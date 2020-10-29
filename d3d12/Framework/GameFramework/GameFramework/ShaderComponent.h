#pragma once

class ShaderComponent
{
public:
	ShaderComponent() = default;
	~ShaderComponent() = default;

	ID3D12PipelineState* GetPSO();
	ID3D12RootSignature* GetRootSignature();

private:
	ID3D12PipelineState* pso{ nullptr };
	ID3D12RootSignature* rootSignature{ nullptr };
};

