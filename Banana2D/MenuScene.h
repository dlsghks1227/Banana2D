#pragma once
#include "IScene.h"

class MenuScene final : public IScene
{
public:
	MenuScene() noexcept(false);
	~MenuScene() = default;

	void OnEnterScene() override;
	void OnExitScene() override;

	void OnUpdate(DX::StepTimer const& timer) override;
	void OnRender(DX::DeviceResources const& device) override;

private:
	std::wstringstream								m_frameInfoText;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>	m_TextColor;

};

