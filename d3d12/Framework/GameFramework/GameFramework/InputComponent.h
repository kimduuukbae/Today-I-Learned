#pragma once

#include "UpdateComponent.h"

class InputComponent : public UpdateComponent
{
public:

	InputComponent();
	~InputComponent();

	void BindInput(unsigned char action, bool pressed, std::function<void()>&& callback);
	void BindAxis(unsigned char input, std::function<void(float)>&& callback);

	virtual void Update(const GameTimer& gt) override;

private:

	void ProcessAction();
	void ProcessAxis();

	std::unordered_map<unsigned char, std::function<void()>> action[2];
	std::unordered_map<unsigned char, std::function<void(float)>> axis;
};

