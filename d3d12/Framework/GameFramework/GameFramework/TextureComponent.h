#pragma once

class Texture;
class Material;

class TextureComponent
{
public:
	TextureComponent();
	~TextureComponent();

	void SetTexture(Texture* texture);
	void AddTexture(Texture* texture);
	void SetMaterial(Material* material);
	void BindingResource(ID3D12GraphicsCommandList* cmdList);

private:
	Texture* texture{ nullptr };
	std::vector<Texture*> tCluster;
	Material* material{ nullptr };
};

