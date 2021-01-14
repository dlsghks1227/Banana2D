#pragma once
class Banana2D
{
	HWND					m_hWnd;

	ID2D1Factory1*			m_pD2DFactory;
	ID2D1Device*			m_pD2DDevice;
	ID2D1DeviceContext*		m_pD2DDeviceContext;

	D3D_FEATURE_LEVEL		m_pFeatureLevels;

	IWICImagingFactory*		m_pWICFactory;

	ID2D1Bitmap*			m_ppBitmap;

	ID2D1HwndRenderTarget*	m_pRenderTarget;
	ID2D1SolidColorBrush*	m_pLightSlateGrayBrush;

public:
	Banana2D();
	~Banana2D();

	HRESULT		Initialize();

	void		RunMessageLoop();

private:
	HRESULT		CreateDeviceIndependentResources();

	HRESULT		CreateDeviceResources();

	void		DiscardDeviceResources();

	HRESULT		OnRender();

	void		OnResize(UINT width, UINT height);

	HRESULT		LoadBitmapFromFile(PCWSTR uri, UINT destinationWidth, UINT destinationHeight);

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

