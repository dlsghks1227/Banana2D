#pragma once
#include "Object.h"

class Object;

class Component
{
public:
	Component(Object* obj) : owner(obj) {};

	virtual void Awake() {};
	virtual void Start() {};

	virtual void OnUpdate() {};
	virtual void OnRender() {};

protected:
	Object* owner;
};