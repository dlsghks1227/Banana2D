#pragma once
#include "Component.h"

class Object
{
public:
	Object() = default;
	~Object() = default;

	Object(Object&&) = default;
	Object& operator = (Object&&) = default;

	Object(Object const&) = delete;
	Object& operator = (Object const&) = delete;

	virtual void Awake();

	virtual void Start();

	virtual void OnUpdate();
	virtual void OnRender();

	template <typename T>
	std::shared_ptr<T> AddComponent()
	{
		// Component ���� �׽�Ʈ
		static_assert(std::is_base_of<Component, T>::value);

		// �ߺ� ������Ʈ üũ
		for (auto& exisitingComponent : m_component)
		{
			// dynamic_pointer_cast�� ����Ͽ� shared_ptr ĳ����
			// https://docs.microsoft.com/ko-kr/cpp/cpp/how-to-create-and-use-shared-ptr-instances?view=msvc-160
			if (std::dynamic_pointer_cast<T>(exisitingComponent))
			{
				return std::dynamic_pointer_cast<T>(exisitingComponent);
			}
		}

		std::shared_ptr<T> newComponent = std::make_shared<T>(this);
		m_component.push_back(newComponent);
		return newComponent;
	}

	template <typename T>
	std::shared_ptr<T> GetComponent()
	{
		// Component ���� �׽�Ʈ
		static_assert(std::is_base_of<Component, T>::value);

		// ������Ʈ �˻�
		for (auto& exisitingComponent : m_component)
		{
			// dynamic_pointer_cast�� ����Ͽ� shared_ptr ĳ����
			// https://docs.microsoft.com/ko-kr/cpp/cpp/how-to-create-and-use-shared-ptr-instances?view=msvc-160
			if (std::dynamic_pointer_cast<T>(exisitingComponent))
			{
				return std::dynamic_pointer_cast<T>(exisitingComponent);
			}
		}

		return nullptr;
	}

private:
	std::vector<std::shared_ptr<Component>>		m_component;
};