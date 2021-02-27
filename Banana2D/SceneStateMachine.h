#pragma once
#include "framework.h"
#include "IScene.h"

// ¾À ÀüÈ¯
// https://thatgamesguy.co.uk/cpp-game-dev-6/

class SceneStateMachine
{
public:
	SceneStateMachine();
	~SceneStateMachine() = default;

	SceneStateMachine(SceneStateMachine&&) = default;
	SceneStateMachine& operator = (SceneStateMachine&&) = default;

	SceneStateMachine(SceneStateMachine const&) = delete;
	SceneStateMachine& operator = (SceneStateMachine const&) = delete;

	void OnUpdate(DX::StepTimer const& timer);
	void OnLateUpdate(DX::StepTimer const& timer);
	void OnRender();

	void Add(std::wstring const& name, std::shared_ptr<IScene> scene);
	void SwitchTo(std::wstring name);

private:
	std::unordered_map<std::wstring, std::shared_ptr<IScene>>	m_scenes;
	std::shared_ptr<IScene>										m_currentScene;
};