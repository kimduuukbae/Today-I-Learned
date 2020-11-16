#pragma once

class Object;

class IComponent
{
public:
	IComponent();
	virtual ~IComponent();

	void SetOwner(Object* owner);
	Object* GetOwner();
protected:
	Object* owner{ nullptr };
};
