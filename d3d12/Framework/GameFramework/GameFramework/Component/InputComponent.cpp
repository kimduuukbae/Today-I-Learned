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
	this->action[pressed].emplace(action, std::move(callback));
}

void InputComponent::BindAxis(unsigned char input, std::function<void(float)>&& callback)
{
	axis.emplace(input, std::move(callback));
}

void InputComponent::Update(const GameTimer& gt)
{
	ProcessAction();
	ProcessAxis();
}

void InputComponent::ProcessAction()
{
	const auto& events{ InputManager::GetInstance()->GetEvents() };

	for (size_t i = 0; i < 2; ++i) {
		const auto& keys{ events[i] };

		for (auto& key : keys) {
			auto [beg, end] = action[i].equal_range(key);

			for (; beg != end; ++beg)
				beg->second();
		}
	}
}

void InputComponent::ProcessAxis()
{
	const auto& events{ InputManager::GetInstance()->GetAxisEvents() };

	for (auto& event : events) {
		const auto& [key, value] = event;
		auto [beg, end] = axis.equal_range(key);
		for (; beg != end; ++beg)
			beg->second(value);
	}
}
