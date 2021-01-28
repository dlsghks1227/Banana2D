#include "Camera.h"

std::unique_ptr<Camera>		g_camera = std::make_unique<Camera>();

Camera::Camera() noexcept :
	m_matrix(D2D1::Matrix3x2F::Identity())
{
}

void Camera::SetTarget(D2D1_POINT_2F const& pos)
{
	m_matrix = D2D1::Matrix3x2F::Translation(pos.x, pos.y);
}