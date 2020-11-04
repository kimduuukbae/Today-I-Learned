#include "stdafx.h"
#include "InputComponent.h"
#include "InputManager.h"


InputComponent::InputComponent()
{
}

InputComponent::~InputComponent()
{
	action[0].clear();
	action[1].clear();
}

void InputComponent::BindInput(unsigned char action, bool pressed, std::function<void()>&& callback)
{
	this->action[pressed].emplace(std::move(action), std::move(callback));
}

void InputComponent::Update(const GameTimer& gt)
{
	ProcessAction();
}

void InputComponent::ProcessAction()
{
	const auto& events{ InputManager::GetInstance()->GetEvents() };

	for (auto& event : events) {
		const auto [key, state] = event;
		auto [beg, end] = action[state].equal_range(key);
		for (; beg != end; ++beg)
			beg->second();
	}
}