#include "framework.h"
#include "Object.h"

void Object::Awake()
{
	for (auto& item : m_component)
	{
		item->Awake();
	}
}

void Object::Start()
{
	for (auto& item : m_component)
	{
		item->Start();
	}
}

void Object::OnUpdate()
{
	for (auto& item : m_component)
	{
		item->OnUpdate();
	}
}

void Object::OnRender()
{
	for (auto& item : m_component)
	{
		item->OnRender();
	}
}