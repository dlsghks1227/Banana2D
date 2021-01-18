#pragma once
#include "StepTimer.h"

__interface IScene
{
	void OnEnterScene();
	void OnExitScene();

	void OnUpdate(DX::StepTimer const& timer);
	void OnRender(ID2D1DeviceContext const& context);
};

class SceneManager
{
public:
	SceneManager();
	~SceneManager() = default;

	SceneManager(SceneManager&&) = default;
	SceneManager& operator = (SceneManager&&) = default;

	SceneManager(SceneManager const&) = delete;
	SceneManager& operator = (SceneManager const&) = delete;

	void Initalize();

	void OnUpdate(DX::StepTimer const& timer);
	void OnRender(ID2D1DeviceContext const& context);

	void EnterScene(std::wstring name);

private:
	void CreateScene(std::wstring name, IScene& scene);

	std::map<std::wstring, IScene*>	m_SceneList;
	IScene*							m_CurrentScene;
};