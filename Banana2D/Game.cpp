#include "Game.h"

Game::Game() noexcept(false) :
	m_CameraPos(D2D1::Point2F(0.0f, 0.0f))
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

	m_texture1 = std::make_shared<DX::Texture>();
	m_texture1->LoadFromFile(L"Image/Squid/Stand/0000.png");

	m_texture2 = std::make_shared<DX::Texture>();
	m_texture2->LoadFromFile(L"Image/Squid/Stand/0001.png");

	m_texture3 = std::make_shared<DX::Texture>();
	m_texture3->LoadFromFile(L"Image/Squid/Stand/0002.png");

	m_text = std::make_shared<DX::Text>();
	m_text->Initialize();

	m_mainScene = std::make_shared<MainScene>();

	// ----- Scene -----
	m_sceneStateMachine.Add(L"main", m_mainScene);
	m_sceneStateMachine.SwitchTo(L"main");
	// -----------------
}

void Game::Tick()
{
	m_timer.Tick([&]()
		{
			Update(m_timer);
			LateUpdate(m_timer);
		});

	Render();
}

void Game::OnDeviceLost()
{
}

void Game::OnDeviceRestored()
{
}

void Game::Update(DX::StepTimer const& timer)
{
	float elapsedTime = static_cast<float>(timer.GetElapsedSeconds());

	auto context = g_deviceResources->GetD2DDeviceContext();
	D2D1_SIZE_F rtSize = context->GetSize();

	D2D1_POINT_2F temp = D2D1::Point2F(0.0f, 0.0f);
	if (g_inputManager->GetKeyPush(DIK_LEFT))	temp = D2D1::Point2F(temp.x - 1.0f, temp.y + 0.0f);
	if (g_inputManager->GetKeyPush(DIK_RIGHT))	temp = D2D1::Point2F(temp.x + 1.0f, temp.y + 0.0f);
	if (g_inputManager->GetKeyPush(DIK_UP))		temp = D2D1::Point2F(temp.x + 0.0f, temp.y - 1.0f);
	if (g_inputManager->GetKeyPush(DIK_DOWN))	temp = D2D1::Point2F(temp.x + 0.0f, temp.y + 1.0f);
	m_CameraPos = D2D1::Point2F(m_CameraPos.x + temp.x, m_CameraPos.y + temp.y);

	m_sceneStateMachine.OnUpdate(timer);
}

void Game::LateUpdate(DX::StepTimer const& timer)
{
	float elapsedTime = static_cast<float>(timer.GetElapsedSeconds());

	auto context = g_deviceResources->GetD2DDeviceContext();

	m_sceneStateMachine.OnLateUpdate(timer);
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

	g_camera->SetTarget(m_CameraPos);

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

	context->DrawLine(D2D1::Point2(-10.0f, 0.0f), D2D1::Point2(10.0f, 0.0f), m_gridColor.Get(), 2.0f);
	context->DrawLine(D2D1::Point2(0.0f, -10.0f), D2D1::Point2(0.0f, 10.0f), m_gridColor.Get(), 2.0f);
#endif

	m_texture1->Draw(
		D2D1::Point2(0.0f, 0.0f), 
		D2D1::SizeF(1.0f, 1.0f), 
		0.0f
	);
	m_texture2->Draw(D2D1::Point2(100.0f, 100.0f), D2D1::SizeF(0.5f, 0.5f), 0.0f);
	m_texture3->Draw(D2D1::Point2(200.0f, 200.0f), D2D1::SizeF(1.0f, 1.0f), 90.0f);
	m_text->Draw(L"Text", D2D1::RectF(100.0f, 100.0f, 200.0f, 200.0f), D2D1::SizeF(1.0f, 1.0f));
	m_text->Draw(L"Banana", D2D1::RectF(100.0f, 100.0f, 200.0f, 200.0f), D2D1::SizeF(1.0f, 1.0f));
	m_text->Draw(L"Asdasdas", D2D1::RectF(100.0f, 100.0f, 200.0f, 200.0f), D2D1::SizeF(1.0f, 1.0f));

	m_sceneStateMachine.OnRender();

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