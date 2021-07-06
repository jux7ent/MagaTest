#pragma once

#include <unordered_map>

#include "Texture.h"
#include "Vector2.h"

template <typename t>
t clamp(t x, t min, t max)
{
	if (x < min) x = min;
	if (x > max) x = max;
	return x;
}

class Drawable {
private:
	bool inversed;
protected:
	Texture* texture;
public:
	Vector2* Position;
	bool Active;

	Drawable(Texture* texture, Vector2* position, bool inversed = false) {
		this->texture = texture;
		this->Position = position;
		this->inversed = inversed;
		Active = true;
	}

	int GetWidth() {
		return texture->width;
	}

	int GetHeight() {
		return texture->height;
	}

	virtual void Draw(uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH]) { // никакой оптимизации
		if (!Active) return;

		if (Position->y + texture->height < 0 || Position->y > SCREEN_HEIGHT ||
			Position->x + texture->width < 0 || Position->x > SCREEN_WIDTH) {
			return;
		}

		for (int i = 0; i < texture->height; ++i) {
			if (Position->y + i < 0 || Position->y + i >= SCREEN_HEIGHT) continue;

			for (int j = 0; j < texture->width; ++j) {
				if (Position->x + j < 0 || Position->x + j >= SCREEN_WIDTH) continue;

				uint32_t pixel;

				if (inversed) {
					pixel = texture->pixels[texture->height - 1 - i][j];
				}
				else {
					pixel = texture->pixels[i][j];
				}


				if (pixel >> 24 != 0) { // if alpha != 0
					buffer[(int)Position->y + i][(int)Position->x + j] = pixel;
				}
			}
		}
	}

	~Drawable() {
	}
};


class Collideable {
private:
	float xBoxScale = 1;
	float yBoxScale = 1;

	float collisionWidth;
	float collisionHeight;

	std::pair<Vector2, Vector2> cachedCollisionBox; // top left point and bottom right point
public:
	Vector2* Position;

	Collideable(Vector2* position, float collisionWidth, float collisionHeight) : Position(position), collisionWidth(collisionWidth), collisionHeight(collisionHeight) {}

	const std::pair<Vector2, Vector2>& GetCollisionBox() {
		cachedCollisionBox.first.x = Position->x + (1 - xBoxScale) * collisionWidth; // top left
		cachedCollisionBox.first.y = Position->y + (1 - yBoxScale) * collisionHeight; // top left

		cachedCollisionBox.second.x = Position->x + collisionWidth * xBoxScale; // bottom right
		cachedCollisionBox.second.y = Position->y + collisionHeight * yBoxScale; // bottom right

		return cachedCollisionBox;
	}

	void SetCollisionBoxScale(float x, float y) {
		xBoxScale = x;
		yBoxScale = y;
	}

	bool CheckCollision(Collideable* collisionWith) {
		const std::pair<Vector2, Vector2> collisionBox = GetCollisionBox();
		const std::pair<Vector2, Vector2> collisionBoxWith = collisionWith->GetCollisionBox();

		if (collisionBox.second.x < collisionBoxWith.first.x ||
			collisionBox.first.x > collisionBoxWith.second.x) {
			return false;
		}

		if (collisionBox.second.y < collisionBoxWith.first.y ||
			collisionBox.first.y > collisionBoxWith.second.y) {
			return false;
		}

		return true;
	}
};

class RigidBody {
private:
	Vector2 velocity;

public:
	Vector2* Position;

	RigidBody(Vector2* position) : Position(position) {}

	void SetVelocity(float x, float y) {
		velocity.Update(x, y);
	}

	void ApplyForce(float dt, Vector2& force) {
		velocity.x += dt * force.x;
		velocity.y += dt * force.y;

		Position->x += dt * velocity.x;
		Position->y += dt * velocity.y;
	}
};


class Text : public Drawable {
private:
	const size_t SYMBOL_HEIGHT = 15;
	const size_t SYMBOL_WIDTH = 12;

	std::string text;
	std::unordered_map<char, uint32_t**> characterTexture; // h=18. w=14. for symbol

	Texture* fontTexture;

	void FillCharacterTextures(Texture* fontTexture) {
		characterTexture.insert(std::make_pair<char, uint32_t**>('0', GetSymbolTexture('0')));
		characterTexture.insert(std::make_pair<char, uint32_t**>('1', GetSymbolTexture('1')));
		characterTexture.insert(std::make_pair<char, uint32_t**>('2', GetSymbolTexture('2')));
		characterTexture.insert(std::make_pair<char, uint32_t**>('3', GetSymbolTexture('3')));
		characterTexture.insert(std::make_pair<char, uint32_t**>('4', GetSymbolTexture('4')));
		characterTexture.insert(std::make_pair<char, uint32_t**>('5', GetSymbolTexture('5')));
		characterTexture.insert(std::make_pair<char, uint32_t**>('6', GetSymbolTexture('6')));
		characterTexture.insert(std::make_pair<char, uint32_t**>('7', GetSymbolTexture('7')));
		characterTexture.insert(std::make_pair<char, uint32_t**>('8', GetSymbolTexture('8')));
		characterTexture.insert(std::make_pair<char, uint32_t**>('9', GetSymbolTexture('9')));
		characterTexture.insert(std::make_pair<char, uint32_t**>(' ', GetSymbolTexture(' ')));
	}

	uint32_t** GetSymbolTexture(char symbol) {
		int num = symbol - '0';

		uint32_t** result = new uint32_t*[SYMBOL_HEIGHT];
		for (int i = 0; i < SYMBOL_HEIGHT; ++i) result[i] = new uint32_t[SYMBOL_WIDTH];

		if (symbol != ' ') {
			for (int i = 0; i < SYMBOL_HEIGHT; ++i) {
				for (int j = 0; j < SYMBOL_WIDTH; ++j) {
					result[i][j] = texture->pixels[50 + i][10 + j + int(num * 10.4)];
					if ((result[i][j] >> 16) - ((result[i][j] >> 24) << 8) > 100) {
						result[i][j] = 0;
					}
				}
			}
		}
		else {
			for (int i = 0; i < SYMBOL_HEIGHT; ++i) {
				for (int j = 0; j < SYMBOL_WIDTH; ++j) {
					result[i][j] = 0;
				}
			}
		}

		return result;
	}

	Texture* MakeTextureFromText(std::string text) {
		int textureHeight = SYMBOL_HEIGHT;
		int textureWidth = SYMBOL_WIDTH * text.size();

		uint32_t** pixels = new uint32_t* [textureHeight];
		for (int i = 0; i < textureHeight; ++i) pixels[i] = new uint32_t[textureWidth];

		for (int i = 0; i < text.size(); ++i) {
			for (int h = 0; h < textureHeight; ++h) {
				for (int w = 0; w < SYMBOL_WIDTH; ++w) {
					pixels[h][SYMBOL_WIDTH * i + w] = characterTexture[text[i]][h][w];
				}
			}
		}

		return new Texture((const uint32_t**)pixels, textureHeight, textureWidth);
	}

public:
	Vector2* Position;

	Text(Texture* fontTexture, Vector2* position) : Position(position), Drawable(fontTexture, position, false) {
		this->fontTexture = fontTexture;
		FillCharacterTextures(fontTexture);

		texture = new Texture(nullptr, 0, 0);
	}

	void SetText(std::string text) {
		this->text = text;

		delete texture;

		texture = MakeTextureFromText(text);
	}
};