#pragma once
#include "IScene.h"

class SceneManager final
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
};