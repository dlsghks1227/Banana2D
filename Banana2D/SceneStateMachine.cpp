#include "SceneStateMachine.h"


SceneStateMachine::SceneStateMachine() :
	m_scenes(),
	m_currentScene()
{
}

void SceneStateMachine::OnUpdate(DX::StepTimer const& timer)
{
	if (m_currentScene)
	{
		m_currentScene->OnUpdate(timer);
	}
}

void SceneStateMachine::OnLateUpdate(DX::StepTimer const& timer)
{
	if (m_currentScene)
	{
		m_currentScene->OnLateUpdate(timer);
	}
}

void SceneStateMachine::OnRender()
{
	if (m_currentScene)
	{
		m_currentScene->OnRender();
	}
}

void SceneStateMachine::Add(std::wstring const& name, std::shared_ptr<IScene> scene)
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

void SceneStateMachine::SwitchTo(std::wstring name)
{
	// map 키 값 검색
	// https://stackoverflow.com/questions/3136520/determine-if-map-contains-a-value-for-a-key

	// 괜찮은 map 설명
	// http://www.vishalchovatiya.com/using-std-map-wisely-with-modern-cpp/#stdmapcontainsC20

	// C++20 이전 코드
	auto itr = m_scenes.find(name);
	if (itr != m_scenes.end())
	{
		if (m_currentScene)
		{
			m_currentScene->OnExitScene();
		}

		m_currentScene = itr->second;

		m_currentScene->OnEnterScene();
	}

	// C++20 사용 가능 코드
	//if (m_scenes.contains(name) == true)
	//{
	//	if (m_currentScene)
	//	{
	//		m_currentScene->OnExitScene();
	//	}

	//	m_currentScene = m_scenes[name];

	//	m_currentScene->OnEnterScene();
	//}
}