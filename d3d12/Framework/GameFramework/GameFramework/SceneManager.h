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
	virtual ~SceneManager();
	
	template <typename T> requires is_base<T>
	void Commit(const std::string& SceneName)
	{
		scenes[SceneName] = make_unique<T>();
		if (!mainScene)
			mainScene = scenes[SceneName].get();
	}
	
private:
	friend FrameworkApp;
	friend class GameplayStatics;

	void Init();
	void Draw(ID3D12GraphicsCommandList* cmdList);
	void Update(const GameTimer& gt);
	void ClearUploadBuffer();

	std::unordered_map<std::string, std::unique_ptr<Scene>> scenes;
	
	Scene* mainScene{ nullptr };

	class MeshManager* meshManager{ nullptr };
	class ResourceManager* resourceManager{ nullptr };
};

