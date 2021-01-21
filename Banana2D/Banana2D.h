#pragma once
#include "DeviceResources.h"
#include "StepTimer.h"

#include "SceneManager.h"

class Banana2D final : public DX::IDeviceNotify
{
public:
	Banana2D() noexcept(false);
	~Banana2D() = default;

	// delete / default 키워드
	// https://blankspace-dev.tistory.com/350
	Banana2D(Banana2D&&) = default;
	Banana2D& operator = (Banana2D&&) = default;

	Banana2D(Banana2D const&) = delete;
	Banana2D& operator = (Banana2D const&) = delete;

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

	// 디바이스 리소스
	std::unique_ptr<DX::DeviceResources>			m_deviceResources;

	// Grid
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>	m_GridColor;

	//// Manage
	std::unique_ptr<SceneManager>					m_SceneManager;

	DX::StepTimer									m_timer;
};