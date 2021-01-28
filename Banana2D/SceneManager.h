#pragma once
#include "framework.h"
#include "IScene.h"

// ¾À ÀüÈ¯
// https://thatgamesguy.co.uk/cpp-game-dev-6/

class SceneManager
{
public:
	SceneManager();
	~SceneManager() = default;

	SceneManager(SceneManager&&) = default;
	SceneManager& operator = (SceneManager&&) = default;

	SceneManager(SceneManager const&) = delete;
	SceneManager& operator = (SceneManager const&) = delete;

	void OnUpdate(DX::StepTimer const& timer);
	void OnRender();

	void Add(std::wstring const& name, std::shared_ptr<IScene> scene);
	void SwitchTo(std::wstring name);

private:
	std::unordered_map<std::wstring, std::shared_ptr<IScene>>	m_scenes;
	std::shared_ptr<IScene>										m_currentScene;
};