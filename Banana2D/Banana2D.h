#pragma once
#include "DeviceResources.h"
#include "StepTimer.h"

class Banana2D final : public DX::IDeviceNotify
{
public:
	Banana2D() noexcept(false);
	~Banana2D() = default;

	// delete / default Ű����
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
	void OnActvated();
	void OnWindowMoved();
	void OnWindowSizeChanged(int width, int height);

	void GetDefaultSize(int& width, int& height) const noexcept;

private:
	void Update(DX::StepTimer const& timer);
	void Render();

	void Clear();

	void CreateDeviceDependentResources();
	void CreateWindowSizeDependentResources();

	// ����̽� ���ҽ�
	std::unique_ptr<DX::DeviceResources>		m_deviceResources;

	DX::StepTimer								m_timer;
};

