#include "Game.h"

Game::Game() noexcept(false)
{
	g_deviceResources->RegisterDeviceNotify(this);
}

void Game::Initialize(HWND window, int width, int height)
{
	g_deviceResources->SetWindow(window, width, height);

	g_deviceResources->CreateDeviceResources();
	CreateDeviceDependentResources();

	g_deviceResources->CreateWindowSizeDependentResources();
	CreateWindowSizeDependentResources();

	m_texture = std::make_shared<DX::Texture>();
	m_texture->LoadFromFile(L"Image/default.png");

	m_text = std::make_shared<DX::Text>();
	m_text->Initialize();
}

void Game::Tick()
{
	m_timer.Tick([&]()
		{
			Update(m_timer);
		});

	Render();
}

void Game::OnDeviceLost()
{
}

void Game::OnDeviceRestored()
{
}

void Game::OnActivated()
{
#ifdef _DEBUG
	std::wcout << "OnActivated()\n";
#endif
}

void Game::OnDeactivated()
{
#ifdef _DEBUG
	std::wcout << "OnDeactivated()\n";
#endif

}

void Game::OnSuspending()
{
#ifdef _DEBUG
	std::wcout << "OnSuspending()\n";
#endif

}

void Game::OnResuming()
{
#ifdef _DEBUG
	std::wcout << "OnResuming()\n";
#endif

	m_timer.ResetElapsedTime();
}

void Game::Update(DX::StepTimer const& timer)
{
	float elapsedTime = static_cast<float>(timer.GetElapsedSeconds());
}

void Game::Render()
{
	if (m_timer.GetFrameCount() == 0)
	{
		return;
	}

	auto context = g_deviceResources->GetD2DDeviceContext();

	D2D1_SIZE_F rtSize = context->GetSize();
	int width = static_cast<int>(rtSize.width);
	int height = static_cast<int>(rtSize.height);

	context->BeginDraw();

	// ----- Clear -----
	context->SetTransform(D2D1::Matrix3x2F::Identity());
	context->Clear(D2D1::ColorF(D2D1::ColorF::White));
	// -----------------

#ifdef _DEBUG
	for (int x = 0; x < width; x += 10)
	{
		context->DrawLine(
			D2D1::Point2(static_cast<FLOAT>(x), 0.0f),
			D2D1::Point2(static_cast<FLOAT>(x), rtSize.height),
			m_gridColor.Get(),
			(x % 50 == 0) ? 1.0f : 0.5f
		);
	}

	for (int y = 0; y < height; y += 10)
	{
		context->DrawLine(
			D2D1::Point2(0.0f, static_cast<FLOAT>(y)),
			D2D1::Point2(rtSize.width, static_cast<FLOAT>(y)),
			m_gridColor.Get(),
			(y % 50 == 0) ? 1.0f : 0.5f
		);
	}
#endif

	m_texture->Draw(D2D1::Point2(0.0f, 0.0f), D2D1::SizeF(0.5f, 0.5f), 0.0f);
	m_text->Draw(L"Text", D2D1::RectF(100.0f, 100.0f, 200.0f, 200.0f), D2D1::SizeF(2.0f, 2.0f));

#ifdef _DEBUG
	context->DrawLine(D2D1::Point2(40.0f, 50.0f), D2D1::Point2(60.0f, 50.0f), m_gridColor.Get(), 2.0f);
	context->DrawLine(D2D1::Point2(50.0f, 40.0f), D2D1::Point2(50.0f, 60.0f), m_gridColor.Get(), 2.0f);
#endif

	DX::ThrowIfFailed(context->EndDraw());

	g_deviceResources->Present();
}

void Game::OnWindowMoved()
{
	auto r = g_deviceResources->GetOutputSize();
	g_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnWindowSizeChanged(int width, int height)
{
	if (!g_deviceResources->WindowSizeChanged(width, height))
		return;

	CreateDeviceDependentResources();
}

void Game::GetDefaultSize(int& width, int& height) const noexcept
{
	width = 1280;
	height = 960;
}

void Game::CreateDeviceDependentResources()
{
	auto context = g_deviceResources->GetD2DDeviceContext();

#ifdef _DEBUG
	DX::ThrowIfFailed(context->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::LightSlateGray),
		m_gridColor.ReleaseAndGetAddressOf()
	));
#endif
}

void Game::CreateWindowSizeDependentResources()
{

}