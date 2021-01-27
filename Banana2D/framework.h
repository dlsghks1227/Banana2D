// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
#include <wincodec.h>

#include <wrl.h>

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory>
#include <stdexcept>
#include <tchar.h>
#include <cstdio>
#include <exception>
#include <vector>

#ifdef _DEBUG
#include <iostream>
#endif

// DirectX apps don't need GDI
#define NODRAWTEXT
#define NOGDI
#define NOBITMAP

// DirectX 헤더 파일입니다.
#include <dxgi1_6.h>
#include <d3d11_4.h>
#include <d2d1_3.h>
#include <dwrite_3.h>

// DirectXMath
// https://docs.microsoft.com/en-us/windows/win32/dxmath/directxmath-portal
#include <DirectXMath.h>
#include <DirectXColors.h>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dwrite.lib")

// 모듈 기본 주소
#ifndef HINST_THISCOMPONENT
EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

// 콘솔창 출력
#ifndef Assert
#if defined( DEBUG ) || defined( _DEBUG )
#include <dxgidebug.h>
#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#define Assert(b) do {if (!(b)) { OutputDebugStringA("Assert: " #b "\n");}} while(0)
#else
#define Assert(b)
#endif // DEBUG || _DEBUG
#endif

// 예외처리
namespace DX
{
	// https://github.com/Microsoft/DirectXTK/wiki/ThrowIfFailed
	// Helper class for COM exceptions

	class com_exception : public std::exception
	{
	public:
		com_exception(HRESULT hr) : result(hr) {}

		const char* what() const override
		{
			static char s_str[64] = {};
			sprintf_s(s_str, "Failure with HRESULT of %08X",
				static_cast<unsigned int>(result));
			return s_str;
		}

	private:
		HRESULT result;
	};

	// Helper utility converts D3D API failures into exceptions.
	inline void ThrowIfFailed(HRESULT hr)
	{
		if (FAILED(hr))
		{
			throw com_exception(hr);
		}
	}
}

// 인터페이스 해제
template<class Interface>
inline UINT32 SafeRelease(Interface** ppInterfaceToRelease)
{
	UINT32 ret = 0;
	::IUnknown* pUnknown = nullptr;

	if (*ppInterfaceToRelease == nullptr)
		return 0;

	pUnknown = dynamic_cast<::IUnknown*>(*ppInterfaceToRelease);

	if (pUnknown == nullptr)
		return 0;

	if (*ppInterfaceToRelease != nullptr)
	{
		ret = (*ppInterfaceToRelease)->Release();
		(*ppInterfaceToRelease) = nullptr;
	}

	return ret;
}