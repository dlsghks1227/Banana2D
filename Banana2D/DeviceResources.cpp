#include "framework.h"

#include "DeviceResources.h"

// 참고자료
// 코딩 기법(1) - https://walbourn.github.io/dual-use-coding-techniques-for-games-part-1/

namespace
{
#if defined( DEBUG ) || defined( _DEBUG )
	// 디버그 계층을 지원하는 디바이스 생성
	// 디버그 모드에서만 동작
	// https://docs.microsoft.com/en-us/windows/win32/direct3d11/overviews-direct3d-11-devices-layers
	inline bool SdkLayersAvailable() noexcept
	{
		HRESULT hr = D3D11CreateDevice(
			nullptr,
			D3D_DRIVER_TYPE_NULL,		//
			nullptr,
			D3D11_CREATE_DEVICE_DEBUG,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			nullptr,
			nullptr,
			nullptr
		);

		return SUCCEEDED(hr);
	}
#endif

	inline DXGI_FORMAT NoSRGB(DXGI_FORMAT fmt) noexcept
	{
		switch (fmt)
		{
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:   return DXGI_FORMAT_R8G8B8A8_UNORM;
		case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:   return DXGI_FORMAT_B8G8R8A8_UNORM;
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:   return DXGI_FORMAT_B8G8R8X8_UNORM;
		default:                                return fmt;
		}
	}
}

DX::DeviceResources::DeviceResources(
	DXGI_FORMAT backBufferFormat,
	DXGI_FORMAT depthBufferFormat,
	UINT backBufferCount,
	D3D_FEATURE_LEVEL minFeatureLevel,
	unsigned int flags = c_FlipPresent) noexcept :
	m_dxgiFactory{},
	m_backBufferFormat(backBufferFormat),
	m_depthBufferFormat(depthBufferFormat),
	m_backBufferCount(backBufferCount),
	m_d3dMinFeatureLevel(minFeatureLevel),
	m_window(nullptr),
	m_d3dFeatureLevel(D3D_FEATURE_LEVEL_9_1),
	m_outputSize{ 0, 0, 1, 1 },
	m_colorSpace(DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709),
	m_options(flags | c_FlipPresent),
	m_d3dDevice(nullptr)
{
}

void DX::DeviceResources::CreateDeviceResources()
{
	UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined( DEBUG ) || defined( _DEBUG )
	if (SdkLayersAvailable())
	{
		// D3D11_CREATE_DEVICE_DEBUG
		// 디버그 계층을 지원하는 장치를 생성
		// https://docs.microsoft.com/en-us/windows/win32/api/d3d11/ne-d3d11-d3d11_create_device_flag
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
	}
	else
	{
		std::wcout << L"WARNING: Direct3D Debug Device is not available\n";
	}
#endif

	CreateFactory();
}

void DX::DeviceResources::CreateFactory()
{
	// _WIN32_WINNT
	// Window SDK를 사용하는 경우 코드를 실행할 수 있는 Windows 버전을 지정할 수 있음
	// - https://docs.microsoft.com/ko-kr/cpp/porting/modifying-winver-and-win32-winnt?view=msvc-160
#if (defined( DEBUG ) || defined( _DEBUG )) && (_WIN32_WINNT >= 0x0603)
	bool debugDXGI = false;
	{
		// DXGI 디버그 디바이스 설정
		// Infomation-Queue 인터페이스는 디버그 메시지를 저장, 검색 및 필터링합니다.
		// Queue는 메시지 큐, 선택적 필터 스택 및 선택적 검색 필터 스택으로 구성
		// 
		// - https://walbourn.github.io/dxgi-debug-device/
		// - https://docs.microsoft.com/en-us/windows/win32/api/d3d11sdklayers/nn-d3d11sdklayers-id3d11infoqueue
		Microsoft::WRL::ComPtr<IDXGIInfoQueue> dxgiInfoQueue;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(dxgiInfoQueue.GetAddressOf()))))
		{
			debugDXGI = true;

			// DX::ThrowIfFailed(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, __uuidof(IDXGIFactory2), (void**)m_dxgiFactory.ReleaseAndGetAddressOf())));
			DX::ThrowIfFailed(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(m_dxgiFactory.ReleaseAndGetAddressOf())));
			
			// SetBreakOnSeverity
			// 중단 될 메시지 심각도 수준을 설정
			// 해당 심각도 수준의 메시지가 저장소 필터를 통과할 때 중단 될 메시지 심각도 수준을 설정
			// - https://docs.microsoft.com/en-us/windows/win32/api/d3d11sdklayers/nf-d3d11sdklayers-id3d11infoqueue-setbreakonseverity
			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
			dxgiInfoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);

			// DXGI_INFO_QUEUE_MESSAGE_ID
			// Infomation-Queue 필터 설정을 위한 디버그 메시지
			// https://docs.microsoft.com/en-us/windows/win32/api/d3d11sdklayers/ne-d3d11sdklayers-d3d11_message_id
			DXGI_INFO_QUEUE_MESSAGE_ID hide[] =
			{
				D3D11_MESSAGE_ID_CREATETEXTURE1D_UNRECOGNIZEDMISCFLAGS,
			};
			
			// DXGI_INFO_QUEUE_FILTER
			// 디버그 메시지 필터
			// D3D11_INFO_QUEUE_FILTER_DESC AllowList	- 허용할 메시지 유형
			// D3D11_INFO_QUEUE_FILTER_DESC DenyList	- 거부할 메시지 유형
			// - https://docs.microsoft.com/en-us/windows/win32/api/d3d11sdklayers/ns-d3d11sdklayers-d3d11_info_queue_filter

			// D3D11_INFO_QUEUE_FILTER_DESC
			// UINT NumlDs			- 허용하거나 거부할 메시지 ID 수
			// D3D11_MESSAGE_ID		- 혀용 또는 거부 할 메시지 ID의 배열
			// - https://docs.microsoft.com/en-us/windows/win32/api/d3d11sdklayers/ns-d3d11sdklayers-d3d11_info_queue_filter_desc
			DXGI_INFO_QUEUE_FILTER filter = {};
			filter.DenyList.NumIDs = static_cast<UINT>(std::size(hide));
			filter.DenyList.pIDList = hide;
			dxgiInfoQueue->AddStorageFilterEntries(DXGI_DEBUG_DXGI, &filter);
		}
	}

	if (!debugDXGI)
#endif
	// ReleaseAndGetAddressOf
	// 연결 된 인터페이스를 해제 한 후 해제된 인터페이스에 대한 포인터를 반환
	// 리소스를 해제하고 다른 포인터 값을 저장할 수 있는 래핑 된 포인터의 주소를 반환
	// MS docs - https://docs.microsoft.com/ko-kr/cpp/cppcx/wrl/comptr-class?view=msvc-160#releaseandgetaddressof
	// stackoverflow - https://stackoverflow.com/questions/13418176/manually-release-comptr
	DX::ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(m_dxgiFactory.ReleaseAndGetAddressOf())));
}