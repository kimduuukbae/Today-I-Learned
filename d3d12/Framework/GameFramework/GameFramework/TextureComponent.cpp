#include "stdafx.h"
#include "TextureComponent.h"
#include "Texture.h"
#include "Material.h"

TextureComponent::TextureComponent()
{
}

TextureComponent::~TextureComponent()
{
}

void TextureComponent::SetTexture(Texture* texture)
{
	this->texture = texture;
}

void TextureComponent::SetMaterial(Material* material)
{
	this->material = material;
}

void TextureComponent::BindingResource(ID3D12GraphicsCommandList* cmdList)
{
	texture->BindingResource(cmdList);
}
