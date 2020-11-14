#include "stdafx.h"
#include "InputManager.h"

InputManager::InputManager()
{

}

InputManager::~InputManager()
{
	events.clear();
}

void InputManager::Init()
{
	events.reserve(64);
}

void InputManager::PushEvent(unsigned char key, unsigned char state)
{
	keys[key] = state;
	events[key] = state;
	if (keys[key] == false)
		events.erase(key);
}

void InputManager::PushAxisEvent(unsigned char key, float axis)
{
	axisEvents[key] = axis;
}

void InputManager::ReleaseEvent()
{
	axisEvents.clear();
}
