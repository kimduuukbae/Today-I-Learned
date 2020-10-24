#pragma once
#include "Object.h"

class CameraComponent;
class SceneManager;

class Scene
{
public:
	Scene() = default;
	virtual ~Scene() {}
	void SetMainCamera(CameraComponent* target);

private:
	friend SceneManager;

	virtual void Init() = 0;
	void Draw();
	void Update(const GameTimer& gt);

	CameraComponent* mainCam{ nullptr };
	std::vector<std::unique_ptr<Object>> objects{ nullptr };
};

