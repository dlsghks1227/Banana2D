#pragma once
#include "IScene.h"

class MainScene final : public IScene
{
public:
	MainScene() noexcept(false);
	~MainScene() = default;

	void OnEnterScene() override;
	void OnExitScene() override;

	void OnUpdate(DX::StepTimer const& timer) override;
	void OnRender(DX::DeviceResources const& device) override;

private:
};