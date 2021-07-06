#include "Vector2.h"

void Vector2::Update(float newX, float newY) {
	x = newX;
	y = newY;
}

Vector2::Vector2(float x, float y) {
	this->x = x;
	this->y = y;
}