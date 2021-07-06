#pragma once

#define _CRT_SECURE_NO_DEPRECATE

#include <fstream>
#include <iostream>
#include <windows.h>
#include <array>
#include <vector>
#include <cstdint>

#include "Engine.h"

class Texture
{
public:
    const uint32_t** pixels;
    const int height;
    const int width;

	Texture(const uint32_t** pixels, int height, int width) : pixels(pixels), height(height), width(width) { }

    ~Texture() {
        if (width != 0 && height != 0) {
            for (int i = 0; i < height; ++i) {
                delete pixels[i];
            }

            delete pixels;
        }
    }
};

