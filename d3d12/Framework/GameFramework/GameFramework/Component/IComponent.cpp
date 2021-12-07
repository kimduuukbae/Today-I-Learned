#include "stdafx.h"
#include "IComponent.h"

IComponent::IComponent(){}

IComponent::~IComponent(){}

void IComponent::SetOwner(Object* owner)
{
	this->owner = owner;
}

Object* IComponent::GetOwner()
{
	return owner;
}
