#pragma once

#include "Texture.h"


class TextureReader {
public:
	virtual Texture* Read(const char* path, float scaleWidth = 1, float scaleHeight = 1) = 0;
};

class BmpReader : public TextureReader {
public:
	Texture* Read(const char* path, float scaleWidth=1, float scaleHeight=1) {
        FILE* f = fopen(path, "rb");

        if (f == NULL) {
            std::cout << "file: " << path << " not found" << std::endl;
            throw "Argument Exception";
        }

        BITMAPFILEHEADER bitmapFileHeader;
        fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, f);
        // std::cout << "SIZE: " << sizeof(BITMAPFILEHEADER) << " " << std::hex << bitmapFileHeader.bfOffBits << std::endl;

        BITMAPINFOHEADER bitmapInfoHeader;
        fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, f);
        // std::cout << "width: " << bitmapInfoHeader.biWidth << " height: " << bitmapInfoHeader.biHeight << " bitCountPerPixel: " << bitmapInfoHeader.biBitCount << std::endl;

        int forOffset = bitmapFileHeader.bfOffBits - (sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER));
        // std::cout << "add for offset: " << forOffset << std::endl;

        char* trash = new char[forOffset];
        fread(trash, sizeof(unsigned char), forOffset, f);

        int width = bitmapInfoHeader.biWidth;
        int height = bitmapInfoHeader.biHeight;

        uint32_t** pixels = new uint32_t * [height];
        for (int i = 0; i < height; ++i) pixels[i] = new uint32_t[width];

        unsigned char* data = new unsigned char[width * 4];

        for (int i = 0; i < height; i++)
        {
            fread(data, sizeof(unsigned char), 4 * width, f);

            for (int j = 0; j < width * 4; j += 4)
            {
                int b = int(data[j]);
                int g = int(data[j + 1]);
                int r = int(data[j + 2]);
                int alpha = int(data[j + 3]);

                uint32_t pixel = (alpha << 24) + (r << 16) + (g << 8) + b;
                pixels[height - 1 - i][j / 4] = pixel;
            };
        }

        fclose(f);

        if (scaleHeight == 1 && scaleWidth == 1) { 
            return new Texture((const uint32_t**)pixels, height, width);
        }
        else {
            int scaledHeight = height * scaleHeight;
            int scaledWidth = width * scaleWidth;

            uint32_t** scaledPixels = new uint32_t*[scaledHeight];
            for (int i = 0; i < scaledHeight; ++i) scaledPixels[i] = new uint32_t[scaledWidth];

            for (int i = 0; i < scaledHeight; ++i) {
                for (int j = 0; j < scaledWidth; ++j) {
                    scaledPixels[i][j] = pixels[int(i / scaleHeight)][int(j / scaleWidth)];
                }
            }

            return new Texture((const uint32_t**)scaledPixels, scaledHeight, scaledWidth);
        }
	}
};