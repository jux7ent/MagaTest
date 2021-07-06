#pragma once

#include "Components.h"

class Tube : public Drawable, public Collideable {
public:
	Vector2* Position;

	Tube(Texture* tubeTexture, Vector2* position, bool inversed) : 
		Position(position),
		Drawable(tubeTexture, position, inversed),
		Collideable(position, tubeTexture->width, tubeTexture->height) {}

	int GetWidth() {
		return texture->width;
	}

	int GetHeight() {
		return texture->height;
	}

	~Tube() {
		delete Position;
	}
};