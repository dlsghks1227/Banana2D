#pragma once
#include "StepTimer.h"

__interface IScene
{
	void OnEnterScene();
	void OnExitScene();

	void OnUpdate(DX::StepTimer const& timer);
	void OnRender();
};