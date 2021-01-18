#pragma once
class Scene
{
public:
	Scene() = default;
	~Scene() = default;

	Scene(Scene&&) = default;
	Scene& operator = (Scene&&) = default;

	Scene(Scene const&) = delete;
	Scene& operator = (Scene const&) = delete;
};