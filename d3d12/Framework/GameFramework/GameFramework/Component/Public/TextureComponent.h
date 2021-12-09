#pragma once

#include "IComponent.h"

class Texture;
class Material;

class TextureComponent : public IComponent
{
public:
	TextureComponent();
	~TextureComponent();

	void SetTexture(Texture* texture);
	void AddTexture(Texture* texture);
	void SetMaterial(Material* material);
	void BindingResource(ID3D12GraphicsCommandList* cmdList);
	size_t GetTClusterSize();

private:
	Texture* texture{ nullptr };
	std::vector<Texture*> tCluster;
	Material* material{ nullptr };
};

