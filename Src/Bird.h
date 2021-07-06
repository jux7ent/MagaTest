#pragma once

#include "Components.h"

class Bird : public Drawable, public Collideable, public RigidBody {
public:
	Vector2* Position;

	Bird(Texture* birdTexture, Vector2* position) : 
		Position(position),
		Drawable(birdTexture, position, false),
		Collideable(position, birdTexture->width, birdTexture->height),
		RigidBody(position) {}

	~Bird() {
		delete Position;
	}
};
