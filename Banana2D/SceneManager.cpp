#include "framework.h"

#include "SceneManager.h"

SceneManager::SceneManager() :
	m_CurrentScene(nullptr)
{

}

void SceneManager::Initalize()
{
}

void SceneManager::OnUpdate(DX::StepTimer const& timer)
{
	if (m_CurrentScene)
	{
		m_CurrentScene->OnUpdate(timer);
	}
}

void SceneManager::OnRender(ID2D1DeviceContext const& context)
{
	if (m_CurrentScene)
	{
		m_CurrentScene->OnRender(context);
	}
}

void SceneManager::EnterScene(std::wstring name)
{
	if (m_SceneList.contains(name) == true)
	{
		IScene* scene = m_CurrentScene;
	}
	else
	{
		throw std::logic_error("[Scene] 존재하지 않은 이름입니다.");
	}
}

void SceneManager::CreateScene(std::wstring name, IScene& scene)
{
	if (m_SceneList.contains(name) == true) 
	{
		throw std::logic_error("[Scene] 이미 존재하는 이름입니다.");
	}
	else
	{
		// Effective C++ 항목 23 : 멤버 함수보다는 비멤버 함수와 더 가까워지자.
		// Effective Modern C++ 항목 13 : iterator보다 const_iterator를 선호하라.
		auto w = std::cend(m_SceneList);
		m_SceneList.insert(w, { name, &scene });
	}
}