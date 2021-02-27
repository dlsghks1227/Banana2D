#pragma once
#include "framework.h"
#include "StepTimer.h"
#include "DeviceResources.h"
#include "Texture.h"
#include "Text.h"

#include "InputManager.h"

#include "SceneStateMachine.h"
#include "MainScene.h"

class Game final : public DX::IDeviceNotify
{
public:
	Game() noexcept(false);
	~Game() = default;

	// delete / default 키워드
	// https://blankspace-dev.tistory.com/350
	Game(Game&&) = default;
	Game& operator = (Game&&) = default;

	Game(Game const&) = delete;
	Game& operator = (Game const&) = delete;

	void Initialize(HWND window, int width, int height);

	void Tick();

	void OnDeviceLost() override;
	void OnDeviceRestored() override;

	void OnWindowMoved();
	void OnWindowSizeChanged(int width, int height);

	void GetDefaultSize(int& width, int& height) const noexcept;

private:
	void Update(DX::StepTimer const& timer);		// 오브젝트 움직임 및 애니메이션 업데이트
	void LateUpdate(DX::StepTimer const& timer);	// 충돌 처리 및 오브젝트 위치가 업데이트 되고나서 해야할 로직들
	void Render();									// 렌더링

	void CreateDeviceDependentResources();
	void CreateWindowSizeDependentResources();

	DX::StepTimer		m_timer;

	// Scene
	SceneStateMachine					m_sceneStateMachine;
	std::shared_ptr<MainScene>			m_mainScene;

	std::shared_ptr<DX::Texture>		m_texture1;
	std::shared_ptr<DX::Texture>		m_texture2;
	std::shared_ptr<DX::Texture>		m_texture3;

	D2D1_POINT_2F						m_CameraPos;


	std::shared_ptr<DX::Text>			m_text;

#ifdef _DEBUG
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush>		m_gridColor;
#endif
};