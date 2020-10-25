#pragma once

template <typename T>
class Singleton
{
public:
	Singleton() = default;
	virtual ~Singleton()
	{
		delete inst;
		inst = nullptr;
	}

	static T* GetInstance()
	{
		if (!inst)
			inst = new T{};
		return inst;
	}

	static void Release()
	{
		delete inst;
		inst = nullptr;
	}

private:
	static T* inst;
};

template <typename T> T* Singleton<T>::inst{ nullptr };

