#pragma once
#include "IScene.h"

class MainScene final : public IScene
{
public:
	MainScene() noexcept(false);
	~MainScene() = default;

	MainScene(MainScene&&) = default;
	MainScene& operator = (MainScene&&) = default;

	MainScene(MainScene const&) = delete;
	MainScene& operator = (MainScene const&) = delete;

	void OnEnterScene();
	void OnExitScene();

	void OnUpdate(DX::StepTimer const& timer);
	void OnRender(ID2D1DeviceContext const& context);

private:
};