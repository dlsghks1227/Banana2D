#pragma once
#include "framework.h"
#include "IResource.h"
#include "DeviceResources.h"

namespace DX
{
	class Texture final : public IResource
	{
	public:
		Texture() noexcept;
		~Texture() = default;

		void LoadFromFile(std::wstring const& path);
		void Draw(
			D2D1_POINT_2F	pos		= D2D1::Point2F(0.0f, 0.0f),
			D2D1_SIZE_F		scale	= D2D1::SizeF(1.0f, 1.0f),
			FLOAT			angle	= 0.0f);

		void SetCenter(bool center) { m_isCenter = center; }
		bool GetCenter()			{ return m_isCenter; }

	private:
		Microsoft::WRL::ComPtr<ID2D1Bitmap1>	m_bitmap;

		D2D1_SIZE_F				m_size;
		D2D1_RECT_F				m_rectangle;


		D2D1_MATRIX_3X2_F		m_rotate;
		D2D1_MATRIX_3X2_F		m_scale;

		bool	m_isCenter;
	};
}