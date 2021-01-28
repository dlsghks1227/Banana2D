#include "Text.h"

using namespace DX;

Text::Text() noexcept :
	m_isCenter(true),
	m_size(D2D1::SizeF(0.0f, 0.0f)),
	m_scale(D2D1::Matrix3x2F::Identity())
{
}

void Text::Initialize()
{
	auto context = g_deviceResources->GetD2DDeviceContext();

	DX::ThrowIfFailed(context->CreateSolidColorBrush(
		D2D1::ColorF(D2D1::ColorF::Black),
		m_color.ReleaseAndGetAddressOf()
	));

	m_size = context->GetSize();
}

void Text::Draw(std::wstring const& text, D2D1_RECT_F rect, D2D1_SIZE_F scale, FLOAT angle)
{
	if (m_isEnable == true)
	{
		auto context = g_deviceResources->GetD2DDeviceContext();
		auto format = g_deviceResources->GetDWTextFormat();

		m_scale = D2D1::Matrix3x2F::Scale(scale, D2D1::Point2F((rect.left + rect.right) * 0.5f, (rect.top + rect.bottom) * 0.5f));

		context->SetTransform(
			m_scale * g_camera->GetMatrix()
		);

#ifdef _DEBUG
		context->DrawRectangle(
			rect,
			m_color.Get()
		);
#endif

		context->DrawTextW(
			text.c_str(),
			static_cast<UINT32>(text.size()),
			format,
			rect,
			m_color.Get()
		);

		context->SetTransform(D2D1::Matrix3x2F::Identity());
	}
}