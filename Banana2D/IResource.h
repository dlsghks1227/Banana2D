#pragma once
#include "framework.h"

namespace DX
{
	class IResource abstract
	{
	public:
		IResource() noexcept : m_isEnable(true) {};
		virtual ~IResource() = default;

		void			SetEnable(bool enable)	{ m_isEnable = enable; }
		bool			GetEnable()				{ return m_isEnable; }
	protected:
		bool			m_isEnable;
	};
}