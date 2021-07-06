#pragma once

#include "Components.h"

class DrawController {
private:
	std::vector<Drawable*> objectsToDraw;
public:
	DrawController();

	void Draw(uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH]);

	template<class Type = Drawable>
	Type* Register(Type* obj) {
		objectsToDraw.push_back(obj);
		return obj;
	}
};