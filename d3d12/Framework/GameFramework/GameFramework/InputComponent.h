#pragma once

#include "IComponent.h"

class InputComponent : public IComponent
{
public:

	InputComponent();
	~InputComponent();

	void BindInput(unsigned char action, bool pressed, std::function<void()>&& callback);

	virtual void Update(const GameTimer& gt) override;

private:

	void ProcessAction();

	std::unordered_map<unsigned char, std::function<void()>> action[2];
};

