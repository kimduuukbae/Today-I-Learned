#pragma once

class Object;

class IComponent
{
public:
	IComponent() = default;
	virtual ~IComponent() = default;

	void SetOwner(Object* owner);
	virtual void Update(const GameTimer& gt) = 0;
protected:
	Object* owner{ nullptr };
};

