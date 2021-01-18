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
		throw std::logic_error("[Scene] �������� ���� �̸��Դϴ�.");
	}
}

void SceneManager::CreateScene(std::wstring name, IScene& scene)
{
	if (m_SceneList.contains(name) == true) 
	{
		throw std::logic_error("[Scene] �̹� �����ϴ� �̸��Դϴ�.");
	}
	else
	{
		// Effective C++ �׸� 23 : ��� �Լ����ٴ� ���� �Լ��� �� ���������.
		// Effective Modern C++ �׸� 13 : iterator���� const_iterator�� ��ȣ�϶�.
		auto w = std::cend(m_SceneList);
		m_SceneList.insert(w, { name, &scene });
	}
}