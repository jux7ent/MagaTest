#pragma once

#include "Components.h"

class Image : public Drawable {
public:
	Vector2* Position;

	Image(Texture* backgroundTexture, Vector2* position) : Position(position), Drawable(backgroundTexture, position) {}

	~Image() {
		delete Position;
	}
};
