#pragma once

class Vector2 {
public:
	float x;
	float y;

	Vector2(float x=0, float y=0);
	void Update(float newX, float newY);
};