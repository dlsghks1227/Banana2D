#pragma once

namespace DX
{
	__interface IDeviceNotify
	{
		void OnDeviceLost();
		void OnDeviceRestored();
	};

	class DeviceResources
	{
	public:
		DeviceResources() noexcept;
		~DeviceResources() = default;

		DeviceResources(DeviceResources&&) = default;
		DeviceResources& operator= (DeviceResources&&) = default;

		DeviceResources(DeviceResources const&) = delete;
		DeviceResources& operator= (DeviceResources const&) = delete;

		void CreateDeviceResources();
		void CreateWindowSizeDependentResources();
		void SetWindow(HWND window, int width, int height) noexcept;
		bool WindowSizeChanged(int width, int height);
		void HandleDeviceLost();
		void RegisterDeviceNotify(IDeviceNotify* deviceNotify) noexcept { m_deviceNotify = deviceNotify; }
		void Present();


		auto				GetDXGIFactory()		const noexcept		{ return m_dxgiFactory.Get(); }
		auto				GetD3DDevice()			const noexcept		{ return m_d3dDevice.Get(); }
		
		auto				GetD2DFactory()			const noexcept		{ return m_d2dFactory.Get(); }
		auto				GetD2DDevice()			const noexcept		{ return m_d2dDevice.Get(); }
		auto				GetD2DDeviceContext()	const noexcept		{ return m_d2dDeviceContext.Get(); }

		auto				GetDWFactory()			const noexcept		{ return m_dwFactory.Get(); }
		auto				GetDWTextFormat()		const noexcept		{ return m_textFormat.Get(); }

		ID2D1Bitmap1*		GetTargetBitmap()		const noexcept		{ return m_d2dTargetBitmap.Get(); }

		HWND				GetWindow()				const noexcept		{ return m_window; }
		D3D_FEATURE_LEVEL	GetDeviceFeatureLevel() const noexcept		{ return m_d3dFeatureLevel; }
		RECT				GetOutputSize()			const noexcept		{ return m_outputSize; }


	private:
		void				CreateFactory();

		// Direct3D objects.
		Microsoft::WRL::ComPtr<IDXGIFactory2>			m_dxgiFactory;
		Microsoft::WRL::ComPtr<ID3D11Device1>			m_d3dDevice;
		Microsoft::WRL::ComPtr<IDXGISwapChain1>			m_dxgiSwapChain;

		// Direct2D objects.
		Microsoft::WRL::ComPtr<ID2D1Factory1>			m_d2dFactory;
		Microsoft::WRL::ComPtr<ID2D1Device>				m_d2dDevice;
		Microsoft::WRL::ComPtr<ID2D1DeviceContext>		m_d2dDeviceContext;

		// DirectWrite objects.
		Microsoft::WRL::ComPtr<IDWriteFactory>			m_dwFactory;
		Microsoft::WRL::ComPtr<IDWriteTextFormat>		m_textFormat;

		// Direct2D rendering objects.
		//Microsoft::WRL::ComPtr<ID3D11Texture2D>		m_renderTarget;
		//Microsoft::WRL::ComPtr<ID2D1RenderTarget>		m_d2dRenderTarget;
		Microsoft::WRL::ComPtr<ID2D1Bitmap1>			m_d2dTargetBitmap;


		// Direct3D properties.

		// Cached device properties.
		HWND											m_window;
		D3D_FEATURE_LEVEL								m_d3dFeatureLevel;
		RECT											m_outputSize;

		IDeviceNotify*									m_deviceNotify;
	};
}