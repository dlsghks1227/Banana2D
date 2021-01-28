#pragma once
#include "framework.h"

class Camera
{
public:
	Camera() noexcept;
	~Camera() = default;

	Camera(Camera&&) = default;
	Camera& operator= (Camera&&) = default;

	Camera(Camera const&) = delete;
	Camera& operator= (Camera const&) = delete;

	D2D1_MATRIX_3X2_F	GetMatrix() { return m_matrix; }

	void				SetTarget(D2D1_POINT_2F const& pos);

private:
	D2D1_MATRIX_3X2_F				m_matrix;
};

extern std::unique_ptr<Camera>		g_camera;