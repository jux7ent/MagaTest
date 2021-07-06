#include "DrawController.h"

DrawController::DrawController() {
}

void DrawController::Draw(uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH]) {
	for (int i = 0; i < objectsToDraw.size(); ++i) objectsToDraw[i]->Draw(buffer);
}