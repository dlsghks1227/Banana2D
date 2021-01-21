#pragma once
#include "StepTimer.h"
#include "DeviceResources.h"

class IScene abstract
{
public:
	IScene() noexcept(false) = default;
	~IScene() = default;

	IScene(IScene&&) = default;
	IScene& operator = (IScene&&) = default;

	IScene(IScene const&) = delete;
	IScene& operator = (IScene const&) = delete;

	virtual void OnEnterScene() = 0;
	virtual void OnExitScene() = 0;

	virtual void OnUpdate(DX::StepTimer const& timer) = 0;
	virtual void OnRender(DX::DeviceResources const& device) = 0;
};