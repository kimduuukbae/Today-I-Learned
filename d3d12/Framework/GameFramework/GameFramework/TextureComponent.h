#pragma once

class Texture;
class Material;

class TextureComponent
{
public:
	TextureComponent();
	~TextureComponent();

	void SetTexture(Texture* texture);
	void SetMaterial(Material* material);
	void BindingResource(ID3D12GraphicsCommandList* cmdList);

private:
	Texture* texture{ nullptr };
	Material* material{ nullptr };
};

