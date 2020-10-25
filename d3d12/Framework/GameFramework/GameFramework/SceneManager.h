#pragma once
#include "Singleton.h"
#include "Scene.h"

class FrameworkApp;

template <typename T>
concept is_base = (std::is_base_of_v<T, Scene> || std::is_base_of_v<Scene, T>);

class SceneManager : public Singleton<SceneManager>
{
public:
	SceneManager() = default;
	~SceneManager() = default;
	
	template <typename T> requires is_base<T>
	void Commit(const std::string& SceneName)
	{
		Scenes[SceneName] = make_unique<T>();
		if (Scenes.size() == 1)
			mainScene = Scenes[SceneName].get();
		Scenes[SceneName]->Init();
	}
	
private:
	friend FrameworkApp;

	void Init();
	void Draw();
	void Update(const GameTimer& gt);

	std::unordered_map<std::string, std::unique_ptr<Scene>> Scenes;
	
	Scene* mainScene{ nullptr };
};

