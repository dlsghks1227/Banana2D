#pragma once
#include "framework.h"
#include "StepTimer.h"

__interface IScene
{
	void OnEnterScene();
	void OnExitScene();

	void OnUpdate(DX::StepTimer const& timer);
	void OnRender(ID2D1DeviceContext const& context);
};