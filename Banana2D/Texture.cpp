#include "Texture.h"

using namespace DX;

Texture::Texture() noexcept :
	m_size(D2D1::SizeF(0.0f, 0.0f)),
	m_rectangle(D2D1::RectF(0.0f, 0.0f, 0.0f, 0.0f)),
	m_rotate(D2D1::Matrix3x2F::Identity()),
	m_scale(D2D1::Matrix3x2F::Identity()),
	m_isCenter(true)
{
}

void Texture::LoadFromFile(std::wstring const& path)
{
	auto wicFactory = g_deviceResources->GetWICFactory();
	auto contect = g_deviceResources->GetD2DDeviceContext();

	Microsoft::WRL::ComPtr<IWICBitmapDecoder>		decoder;
	Microsoft::WRL::ComPtr<IWICBitmapFrameDecode>	source;
	Microsoft::WRL::ComPtr<IWICStream>				stream;
	Microsoft::WRL::ComPtr<IWICFormatConverter>		converter;
	Microsoft::WRL::ComPtr<IWICBitmapScaler>		Scaler;

	DX::ThrowIfFailed(wicFactory->CreateDecoderFromFilename(
		path.c_str(),
		nullptr,
		GENERIC_READ,
		WICDecodeMetadataCacheOnLoad,
		decoder.ReleaseAndGetAddressOf()
	));

	DX::ThrowIfFailed(wicFactory->CreateFormatConverter(
		converter.ReleaseAndGetAddressOf()
	));

	DX::ThrowIfFailed(decoder->GetFrame(0, source.ReleaseAndGetAddressOf()));

	DX::ThrowIfFailed(converter->Initialize(
		source.Get(),
		GUID_WICPixelFormat32bppPBGRA,
		WICBitmapDitherTypeNone,
		nullptr,
		0.0f,
		WICBitmapPaletteTypeCustom
	));

	DX::ThrowIfFailed(contect->CreateBitmapFromWicBitmap(
		converter.Get(),
		nullptr,
		m_bitmap.ReleaseAndGetAddressOf()
	));

	m_size = m_bitmap.Get()->GetSize();
	m_rectangle = D2D1::RectF(0.0f, 0.0f, m_size.width, m_size.height);
}

void Texture::Draw(D2D1_POINT_2F pos, D2D1_SIZE_F scale, FLOAT angle, bool staticPos)
{
	if (m_bitmap)
	{

		if (m_isEnable == true)
		{
			auto context = g_deviceResources->GetD2DDeviceContext();

			m_rectangle = D2D1::RectF(
				pos.x - (m_size.width  * (m_isCenter ? 0.5f : 0.0f)),
				pos.y - (m_size.height * (m_isCenter ? 0.5f : 0.0f)),
				pos.x + (m_size.width  * (m_isCenter ? 0.5f : 1.0f)),
				pos.y + (m_size.height * (m_isCenter ? 0.5f : 1.0f)));

			m_scale		= D2D1::Matrix3x2F::Scale(scale, D2D1::Point2F(pos.x, pos.y));
			m_rotate	= D2D1::Matrix3x2F::Rotation(angle, 
				D2D1::Point2F(
					pos.x + (m_size.width  * (m_isCenter ? 0.0f : 0.5f)),
					pos.y + (m_size.height * (m_isCenter ? 0.0f : 0.5f))
				));

			context->SetTransform(
				m_rotate * m_scale * (staticPos ? D2D1::Matrix3x2F::Identity() : g_camera->GetMatrix())
			);
					
			context->DrawBitmap(
				m_bitmap.Get(),
				m_rectangle,
				1.0f,
				D2D1_BITMAP_INTERPOLATION_MODE_LINEAR
			);
			context->SetTransform(D2D1::Matrix3x2F::Identity());
		}
	}
}
