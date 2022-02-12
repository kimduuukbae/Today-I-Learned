#pragma once

#include "IComponent.h"

class UpdateComponent : public IComponent
{
public:
	UpdateComponent();
	virtual ~UpdateComponent();

	virtual void Update(const GameTimer& gt) = 0;
};

