#pragma once
//// 여러가지 디자인 패턴 구현
//// http://www.vishalchovatiya.com/state-design-pattern-in-modern-cpp/
//// std::optional, std::variant 사용하여 상태패턴 구현
//
//#pragma region Event
//class StartGame {};
//class StopGame {};
//
//using Event = std::variant<StartGame, StopGame>;
//#pragma endregion
//
//#pragma region State
//#include "MenuScene.h"
//#include "MainScene.h"
//
//
//using State = std::variant<MenuScene, MainScene>;
//#pragma endregion
//
//#pragma region Transitions
//class Transitions
//{
//public:
//	std::optional<State> operator()(MenuScene&, const StartGame& e)
//	{
//#ifdef _DEBUG
//		std::wcout << L"Menu -> Main\n";
//#endif
//		return MainScene{};
//	}
//
//	std::optional<State> operator()(MainScene&, const StopGame& e)
//	{
//#ifdef _DEBUG
//		std::wcout << L"Main -> Menu\n";
//#endif
//		return MenuScene{};
//	}
//
//	template <typename State_t, typename Event_t>
//	std::optional<State> operator()(State_t&, const Event_t&) const
//	{
//#ifdef _DEBUG
//		std::wcout << L"null\n";
//#endif
//		return std::nullopt;
//	}
//};
//#pragma endregion
//
//template <typename StateVariant, typename EventVarient, typename Transitions>
//class SceneManager final
//{
//public:
//	SceneManager() = default;
//	~SceneManager() = default;
//
//	SceneManager(SceneManager&&) = default;
//	SceneManager& operator = (SceneManager&&) = default;
//
//	SceneManager(SceneManager const&) = delete;
//	SceneManager& operator = (SceneManager const&) = delete;
//
//	void OnUpdate(DX::StepTimer const& timer)
//	{
//	}
//
//	void OnRender(ID2D1DeviceContext const& context)
//	{
//	}
//
//	void Dispatch(const EventVarient& e) {
//		std::optional<StateVariant> newState = std::visit(Transitions{}, m_currentState, e);
//		if (newState)
//		{
//			m_currentState = *std::move(newState);
//		}
//	}
//
//public:
//	StateVariant m_currentState;
//};
// 실패
// variant의 OnUpdate, OnRender 함수를 불러올 수 있지만 다 설정해야한다는 불편함이 있어서 패스
// 상태 패턴을 구현하는데 편하지만 씬을 관리하는데 불편함

#include "IScene.h"

// 씬 전환
// https://thatgamesguy.co.uk/cpp-game-dev-6/

class SceneManager final
{
public:
	SceneManager();
	~SceneManager() = default;

	SceneManager(SceneManager&&) = default;
	SceneManager& operator = (SceneManager&&) = default;

	SceneManager(SceneManager const&) = delete;
	SceneManager& operator = (SceneManager const&) = delete;

	void OnUpdate(DX::StepTimer const& timer);
	void OnRender(DX::DeviceResources const& device);

	void Add(std::wstring const& name, std::shared_ptr<IScene> scene);
	void SwitchTo(std::wstring name);

private:
	std::unordered_map<std::wstring, std::shared_ptr<IScene>>	m_scenes;
	std::shared_ptr<IScene>										m_currentScene;
	std::string													m_insertedSceneName;
};