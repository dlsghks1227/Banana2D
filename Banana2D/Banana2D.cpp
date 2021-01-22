#include "framework.h"
#include "Banana2D.h"


Banana2D::Banana2D() noexcept(false)
{
	m_deviceResources = std::make_unique<DX::DeviceResources>();
	m_deviceResources->RegisterDeviceNotify(this);

	m_SceneManager = std::make_unique<SceneManager>();
}

void Banana2D::Initialize(HWND window, int width, int height)
{
	m_deviceResources->SetWindow(window, width, height);

	m_deviceResources->CreateDeviceResources();
	CreateDeviceDependentResources();

	m_deviceResources->CreateWindowSizeDependentResources();
	CreateWindowSizeDependentResources();


	// 프레임 제한
	//m_timer.SetFixedTimeStep(true);
	//m_timer.SetTargetElapsedSeconds(1.0 / 60.0);
	//m_SceneManager.Dispatch(StartGame());

	for (int i = 0; i < 4; i++) {
		std::wstringstream str(L"");
		str << L"Image/Squid/Stand/" << std::setfill(L'0') << std::setw(4) << i << L".png";
		ID2D1Bitmap* temp;
		m_deviceResources->LoadBitmapFromFile(str.str().c_str(), 0, 0, &temp);
		m_Bitmaps.push_back(std::move(temp));
		SafeRelease(&temp);
	}
	//m_deviceResources->LoadBitmapFromFile(L"Image/Squid/Stand/0000.png", 0, 0, &m_BitmapImage);
	//m_deviceResources->LoadBitmapFromFile(L"Image/Squid/Stand/0001.png", 0, 0, m_BitmapImage2.ReleaseAndGetAddressOf());
	//m_deviceResources->LoadBitmapFromFile(L"Image/Squid/Stand/0002.png", 0, 0, m_BitmapImage3.ReleaseAndGetAddressOf());

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

void Banana2D::OnActivated()
{
#ifdef _DEBUG
	std::wcout << "OnActivated()\n";
#endif
}

void Banana2D::OnDeactivated()
{
#ifdef _DEBUG
	std::wcout << "OnDeactivated()\n";
#endif

}

void Banana2D::OnSuspending()
{
#ifdef _DEBUG
	std::wcout << "OnSuspending()\n";
#endif

}

void Banana2D::OnResuming()
{
#ifdef _DEBUG
	std::wcout << "OnResuming()\n";
#endif

	m_timer.ResetElapsedTime();
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
	float elapsedTime = static_cast<float>(timer.GetElapsedSeconds());

	//m_frameInfoText.str(L"");
	//m_frameInfoText << "FPS : " << timer.GetFramesPerSecond();

	m_SceneManager->OnUpdate(timer);
}

void Banana2D::Render()
{
	if (m_timer.GetFrameCount() == 0)
	{
		return;
	}

	auto context = m_deviceResources->GetD2DDeviceContext();
	auto textFormat = m_deviceResources->GetDWTextFormat();

	D2D1_SIZE_F rtSize = context->GetSize();
	int width = static_cast<int>(rtSize.width);
	int height = static_cast<int>(rtSize.height);

	context->BeginDraw();

	// ----- Clear -----
	context->SetTransform(D2D1::Matrix3x2F::Identity());
	context->Clear(D2D1::ColorF(D2D1::ColorF::White));
	// -----------------

	m_SceneManager->OnRender((*m_deviceResources));

	//context->DrawTextW(
	//	m_frameInfoText.str().c_str(),
	//	static_cast<UINT32>(m_frameInfoText.str().size()),
	//	textFormat,
	//	D2D1::RectF(10.0f, 10.0f, rtSize.width, rtSize.height),
	//	m_TextColor.Get()
	//);

	for (int x = 0; x < width; x += 10)
	{
		context->DrawLine(
			D2D1::Point2(static_cast<FLOAT>(x), 0.0f),
			D2D1::Point2(static_cast<FLOAT>(x), rtSize.height),
			m_GridColor.Get(),
			(x % 50 == 0) ? 1.0f : 0.5f
		);
	}

	for (int y = 0; y < height; y += 10)
	{
		context->DrawLine(
			D2D1::Point2(0.0f, static_cast<FLOAT>(y)),
			D2D1::Point2(rtSize.width, static_cast<FLOAT>(y)),
			m_GridColor.Get(),
			(y % 50 == 0) ? 1.0f : 0.5f
		);
	}

	float temp = 100.0f;
	for (auto& item : m_Bitmaps) {
		D2D1_SIZE_F size = item.Get()->GetSize();
		D2D1_POINT_2F center = D2D1::Point2(temp - (size.width * 0.5f), temp - (size.height * 0.5f));
		float scale = 1.0f;

		context->DrawBitmap(
			item.Get(),
			D2D1::RectF(
				center.x,
				center.y,
				center.x + (size.width * scale),
				center.y + (size.height * scale)
			),
			1.0f,
			D2D1_BITMAP_INTERPOLATION_MODE_LINEAR
		);
	}

	//if (m_BitmapImage)
	//{
	//	D2D1_SIZE_F size = m_BitmapImage->GetSize();
	//	D2D1_POINT_2F pos = D2D1::Point2(10.0f, 10.0f);
	//	float scale = 1.0f;

	//	context->DrawBitmap(
	//		m_BitmapImage,
	//		D2D1::RectF(
	//			pos.x,
	//			pos.y,
	//			pos.x + (size.width * scale),
	//			pos.y + (size.height * scale)
	//		),
	//		1.0f,
	//		D2D1_BITMAP_INTERPOLATION_MODE_LINEAR
	//	);
	//}

	DX::ThrowIfFailed(context->EndDraw());

	m_deviceResources->Present();
}

void Banana2D::CreateDeviceDependentResources()
{
	auto context = m_deviceResources->GetD2DDeviceContext();

	DX::ThrowIfFailed(context->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::LightSlateGray),
		m_GridColor.ReleaseAndGetAddressOf()
	));
}

void Banana2D::CreateWindowSizeDependentResources()
{

}