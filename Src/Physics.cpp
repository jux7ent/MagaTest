#include "Physics.h"

Physics::Physics(float gravityX, float gravityY) {
	gravityForce.Update(gravityX, gravityY);
}

void Physics::Update(float dt) {
	for (int i = 0; i < physicalObjects.size(); ++i) physicalObjects[i]->ApplyForce(dt, gravityForce);
}