#pragma once

#include <vector>

#include "Components.h"


class Physics {
private:
	std::vector<RigidBody*> physicalObjects;
	Vector2 gravityForce;
public:
	Physics(float gravityX, float gravityY);
	void Update(float dt);

	template<class Type=RigidBody*>
	Type* Register(Type* obj) {
		physicalObjects.push_back(obj);
		return obj;
	}
};