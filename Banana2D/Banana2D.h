#pragma once
class Banana2D
{
	HWND					m_hWnd;

	IDXGIFactory*			m_pDxgiFactory;

	ID2D1Factory*			m_pDirect2dFactory;
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

	void OnResize(UINT width, UINT height);

	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

