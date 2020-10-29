#include "stdafx.h"
#include "IComponent.h"

void IComponent::SetOwner(Object* owner)
{
	this->owner = owner;
}
