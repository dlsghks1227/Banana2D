#pragma once
#include "framework.h"
#include "StepTimer.h"
#include "DeviceResources.h"
#include "Texture.h"
#include "Text.h"

#include "InputManager.h"

class Game final : public DX::IDeviceNotify
{
public:
	Game() noexcept(false);
	~Game() = default;

	// delete / default Å°¿öµå
	// https://blankspace-dev.tistory.com/350
	Game(Game&&) = default;
	Game& operator = (Game&&) = default;

	Game(Game const&) = delete;
	Game& operator = (Game const&) = delete;

	void Initialize(HWND window, int width, int height);

	void Tick();

	void OnDeviceLost() override;
	void OnDeviceRestored() override;

	// Message
	void OnActivated();
	void OnDeactivated();
	void OnSuspending();
	void OnResuming();

	void OnWindowMoved();
	void OnWindowSizeChanged(int width, int height);

	void GetDefaultSize(int& width, int& height) const noexcept;

private:
	void Update(DX::StepTimer const& timer);
	void Render();

	void CreateDeviceDependentResources();
	void CreateWindowSizeDependentResources();

	DX::StepTimer		m_timer;

	std::shared_ptr<DX::Texture>		m_texture1;
	std::shared_ptr<DX::Texture>		m_texture2;
	std::shared_ptr<DX::Texture>		m_texture3;

	D2D1_POINT_2F						m_CameraPos;


	std::shared_ptr<DX::Text>			m_text;

#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>		m_gridColor;
#endif
};