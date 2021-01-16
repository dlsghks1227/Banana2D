#include "framework.h"
#include "Banana2D.h"


Banana2D::Banana2D() noexcept(false)
{
	m_deviceResources = std::make_unique<DX::DeviceResources>();
	m_deviceResources->RegisterDeviceNotify(this);
}

void Banana2D::Initialize(HWND window, int width, int height)
{
	m_deviceResources->SetWindow(window, width, height);

	m_deviceResources->CreateDeviceResources();
	CreateDeviceDependentResources();

	m_deviceResources->CreateWindowSizeDependentResources();
	CreateWindowSizeDependentResources();
}

void Banana2D::Tick()
{
	m_timer.Tick([&]()
		{
			Update(m_timer);
		});

	Render();
}

void Banana2D::OnDeviceLost()
{

}

void Banana2D::OnDeviceRestored()
{
}

void Banana2D::OnWindowMoved()
{
	auto r = m_deviceResources->GetOutputSize();
	m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Banana2D::OnWindowSizeChanged(int width, int height)
{
	if (!m_deviceResources->WindowSizeChanged(width, height))
		return;

	CreateDeviceDependentResources();
}

void Banana2D::GetDefaultSize(int& width, int& height) const noexcept
{
	width = 1280;
	height = 960;
}

void Banana2D::Update(DX::StepTimer const& timer)
{
	float elapsedTime = float(timer.GetElapsedSeconds());
}

void Banana2D::Render()
{
	if (m_timer.GetFrameCount() == 0)
	{
		return;
	}

	Clear();

	auto context = m_deviceResources->GetD2DDeviceContext();

	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> pBlackBrush;
	DX::ThrowIfFailed(
		context->CreateSolidColorBrush(
			D2D1::ColorF(D2D1::ColorF::White),
			&pBlackBrush
		)
	);

	context->BeginDraw();

	context->DrawRectangle(
		D2D1::RectF(
			100.0f,
			100.0f,
			200.0f,
			200.0f),
		pBlackBrush.Get(), 2.0f);

	DX::ThrowIfFailed(context->EndDraw());

	m_deviceResources->Present();
}

void Banana2D::Clear()
{
	auto renderTarget = m_deviceResources->GetRenderTarget();
}

void Banana2D::CreateDeviceDependentResources()
{

}

void Banana2D::CreateWindowSizeDependentResources()
{

}