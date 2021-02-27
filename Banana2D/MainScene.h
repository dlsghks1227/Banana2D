#pragma once
#include "IScene.h"

class MainScene final : public IScene
{
public:
	MainScene()	noexcept;
	~MainScene() = default;

	MainScene(MainScene&&) = default;
	MainScene& operator = (MainScene&&) = default;

	MainScene(MainScene const&) = delete;
	MainScene& operator = (MainScene const&) = delete;

	void OnEnterScene()								override;
	void OnExitScene()								override;

	void OnUpdate(DX::StepTimer const& timer)		override;
	void OnLateUpdate(DX::StepTimer const& timer)	override;

	void OnRender()									override;
private:

};

