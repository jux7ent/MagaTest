#pragma once

#include "Tube.h"


class TubesPair {
private:
	int distanceBetweenTubes;
	int yOffset;

public:
	Tube* topTube;
	Tube* bottomTube;

	TubesPair(Tube* topTube, Tube* bottomTube, int distanceBetweenTubes, int yOffset=0) {
		this->topTube = topTube;
		this->bottomTube = bottomTube;
		this->yOffset = yOffset;
		SetGapeWidth(distanceBetweenTubes);
	}

	void SetGapeWidth(int newVal) {
		distanceBetweenTubes = newVal;

		topTube->Position->y = -topTube->GetHeight() + SCREEN_HEIGHT / 2 - distanceBetweenTubes / 2 + yOffset;
		bottomTube->Position->y = SCREEN_HEIGHT/2 + distanceBetweenTubes / 2 + yOffset;
	}

	void SetYOffset(int newYOffset) {
		yOffset = newYOffset;
		SetGapeWidth(distanceBetweenTubes);
	}

	void SetXPosition(float x) {
		topTube->Position->x = x;
		bottomTube->Position->x = x;
	}

	float GetXPosition() {
		return topTube->Position->x;
	}

	void MoveXAxis(float dm) {
		topTube->Position->x += dm;
		bottomTube->Position->x += dm;
	}

	float RightXPosition() {
		return topTube->Position->x + topTube->GetWidth();
	}

	~TubesPair() {
		delete topTube;
		delete bottomTube;
	}
};
