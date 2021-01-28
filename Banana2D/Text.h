#pragma once
#include "framework.h"
#include "IResource.h"
#include "DeviceResources.h"
#include "Camera.h"

namespace DX
{
	class Text final : public IResource
	{
	public:
		Text() noexcept;
		~Text() = default;

		void Initialize();
		void Draw(
			std::wstring const& text,
			D2D1_RECT_F			rect	= D2D1::RectF(0.0f, 0.0f, 0.0f, 0.0f),
			D2D1_SIZE_F			scale	= D2D1::SizeF(1.0f, 1.0f),
			FLOAT				angle	= 0.0f);

		void SetCenter(bool center) { m_isCenter = center; }
		bool GetCenter()			{ return m_isCenter; }

	private:
		Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>	m_color;

		D2D1_SIZE_F				m_size;

		D2D1_MATRIX_3X2_F		m_scale;

		bool	m_isCenter;
	};
}