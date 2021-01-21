#include "framework.h"
#include "SceneManager.h"

#include "MenuScene.h"
#include "MainScene.h"

SceneManager::SceneManager() :
	m_scenes(),
	m_currentScene()
{
	std::shared_ptr<MenuScene> menuScene = std::make_shared<MenuScene>();
	std::shared_ptr<MainScene> mainScene = std::make_shared<MainScene>();

	Add(L"Menu", menuScene);
	Add(L"Main", mainScene);

	SwitchTo(L"Menu");
}

void SceneManager::OnUpdate(DX::StepTimer const& timer)
{
	if (m_currentScene)
	{
		m_currentScene->OnUpdate(timer);
	}
}

void SceneManager::OnRender(DX::DeviceResources const& device)
{
	if (m_currentScene)
	{
		m_currentScene->OnRender(device);
	}
}

void SceneManager::Add(std::wstring const& name, std::shared_ptr<IScene> scene)
{
	// map insert 정확하게 사용하기
	// https://yonmy.com/archives/9
	auto inserted = m_scenes.insert({ name, scene });
	if (inserted.second == false)
	{
#ifdef _DEBUG
		std::wcout << "[Scene] 중복된 씬입니다.\n";
#endif
		inserted.first->second = scene;
	}
}

void SceneManager::SwitchTo(std::wstring name)
{
	// map 키 값 검색
	// https://stackoverflow.com/questions/3136520/determine-if-map-contains-a-value-for-a-key

	// 괜찮은 map 설명
	// http://www.vishalchovatiya.com/using-std-map-wisely-with-modern-cpp/#stdmapcontainsC20
	if (m_scenes.contains(name) == true)
	{
		if (m_currentScene)
		{
			m_currentScene->OnExitScene();
		}

		m_currentScene = m_scenes[name];

		m_currentScene->OnEnterScene();
	}
}