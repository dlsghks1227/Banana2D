#pragma once

namespace DX
{

	// __interface
	// https://docs.microsoft.com/ko-kr/cpp/cpp/interface?view=msvc-160
	__interface IDeviceNotify
	{
		void OnDeviceLost();
		void OnDeviceRestored();
	};

	class DeviceResources
	{
	public:
		static const unsigned int c_FlipPresent		= 0x1;
		static const unsigned int c_AllowTearing	= 0x2;
		static const unsigned int c_EnableHDR		= 0x4;

		// noexcept
		// 함수에서 예외를 throw 할 수 있는지 여부를 지정
		// https://docs.microsoft.com/ko-kr/cpp/cpp/noexcept-cpp?view=msvc-160
		DeviceResources(
			DXGI_FORMAT backBufferFormat = DXGI_FORMAT_B8G8R8A8_UNORM,
			DXGI_FORMAT depthBufferFormat = DXGI_FORMAT_D32_FLOAT,
			UINT backBufferCount = 2,
			D3D_FEATURE_LEVEL minFeatureLevel = D3D_FEATURE_LEVEL_10_0,
			unsigned int flags = c_FlipPresent) noexcept;
		~DeviceResources() = default;

		// "delete" and "default"
		// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2210.html#%22default%22

		// lValue, rValue
		// https://dydtjr1128.github.io/cpp/2019/06/10/Cpp-values.html
		DeviceResources(DeviceResources&&) = default;
		DeviceResources& operator = (DeviceResources&&) = default;

		DeviceResources(DeviceResources const&) = delete;
		DeviceResources& operator = (DeviceResources const&) = delete;

		void CreateDeviceResources();
		void CreateWindowSizeDependentResoureces();
		void SetWindow(HWND window, int width, int height) noexcept;
		bool WindowSizeChanged(int width, int height);
		void HandleDeviceLost();
		void RegisterDeviceNotify(IDeviceNotify* deviceNotify) noexcept { ; }
		void Present();

	private:
		void CreateFactory();
		void GetHardwareAdapter(IDXGIAdapter1** ppAdapter);
		void UpdateColorSpace();

		// Direct3D objects.
		Microsoft::WRL::ComPtr<IDXGIFactory2>				m_dxgiFactory;
		Microsoft::WRL::ComPtr<ID3D11Device1>				m_d3dDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext1>		m_d3dDeviceContext;
		Microsoft::WRL::ComPtr<IDXGISwapChain1>				m_swapChain;
		Microsoft::WRL::ComPtr<ID3DUserDefinedAnnotation>	m_d3dAnnotation;

		// Direct3D rendering objects. Required for 3D.

		// Direct3D properties.
		DXGI_FORMAT				m_backBufferFormat;
		DXGI_FORMAT				m_depthBufferFormat;
		UINT					m_backBufferCount;
		D3D_FEATURE_LEVEL		m_d3dMinFeatureLevel;

		// Cached device properties.
		HWND					m_window;
		D3D_FEATURE_LEVEL		m_d3dFeatureLevel;
		RECT					m_outputSize;

		// HDR Support
		DXGI_COLOR_SPACE_TYPE	m_colorSpace;

		// DeviceResources options (see flag above)
		unsigned int			m_options;

		// The IDeviceNotify can be held directly as it owns the DeviceResources.
		IDeviceNotify*			m_deviceNotify;
	};
}