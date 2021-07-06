#include <stdlib.h>
#include <memory.h>
#include <vector>
#include <time.h>
#include <fstream>
#include <string>

#include "Texture.h"
#include "Engine.h"
#include "DrawController.h"
#include "Physics.h"
#include "TubesPair.h"
#include "TextureReader.h"
#include "Bird.h"
#include "Image.h"

#define JumpVelocity 350
#define GravityForce 900
#define BirdXPosition 100
#define StartBirdSpeed 200
#define IncreaseBirdSpeedCoeff 2

#define TubeYPos 500
#define InversedTubeYPos -200

#define DefaultTubesGape 400
#define TubesGapeConstriction 200

#define TubesCount 7
#define TubesXOffset 600
#define TubesXOffsetAdditionalRange 300

#define ScoreXPosition 30
#define ScoreYPosition SCREEN_HEIGHT - 100

enum EGameState {
    Game,
    Pause,
    Lose
};

BmpReader bmpReader;
Physics physics(0, GravityForce);
DrawController drawController;

Image* background = nullptr;
Image* pressR = nullptr;
Text* scoreText = nullptr;
Bird* bird = nullptr;


Texture* birdTexture;
Texture* tubeTexture;
Texture* backgroundTexture;
Texture* pressRTexture;
Texture* fontTexture;

EGameState gameState = EGameState::Game;

int score = 0;

std::vector<TubesPair*> tubes;
TubesPair* lastReusedTube = nullptr;
int nearestTubesPairIndex;

float birdSpeed = StartBirdSpeed;


void SetScore(int newScore) {
    score = newScore;
    scoreText->SetText(std::to_string(newScore));
}


void SetNewTubesPairXPosition(int tubeIndex) {
    float lastTubeXPosition;
    if (lastReusedTube == nullptr) {
        lastTubeXPosition = 200;
    }
    else {
        lastTubeXPosition = lastReusedTube->GetXPosition();
    }

    tubes[tubeIndex]->SetXPosition(lastTubeXPosition + TubesXOffset - rand() % TubesXOffsetAdditionalRange);
    lastReusedTube = tubes[tubeIndex];
}

void InitTubesPool(int tubesCount) {
    for (int i = 0; i < tubesCount; ++i) {
        Tube* topTube = drawController.Register(new Tube(tubeTexture, new Vector2(), true));
        Tube* bottomTube = drawController.Register(new Tube(tubeTexture, new Vector2(), false));

        topTube->SetCollisionBoxScale(0.9, 0.98);
        bottomTube->SetCollisionBoxScale(0.9, 0.98);

        TubesPair* tubesPair = new TubesPair(topTube, bottomTube, DefaultTubesGape + -(rand() % TubesGapeConstriction), rand() % 200 - 100);

        tubes.push_back(tubesPair);

        SetNewTubesPairXPosition(i);
    }

    nearestTubesPairIndex = 0;
}

void InitTextures() {
    backgroundTexture = bmpReader.Read("Sprites/background.bmp");
    tubeTexture = bmpReader.Read("Sprites/tube.bmp", 1.2, 1.2);
    birdTexture = bmpReader.Read("Sprites/bird.bmp");
    fontTexture = bmpReader.Read("Sprites/Roboto-Black.bmp");
    pressRTexture = bmpReader.Read("Sprites/pressR.bmp");
}

// initialize game data in this function
void initialize()
{
    srand(time(NULL));

    InitTextures();

    background = drawController.Register(new Image(backgroundTexture, new Vector2()));

    InitTubesPool(TubesCount);

    bird = drawController.Register(physics.Register(new Bird(birdTexture, new Vector2(BirdXPosition, 10))));
    bird->SetCollisionBoxScale(0.8, 0.9);

    scoreText = drawController.Register(new Text(fontTexture, new Vector2()));
    scoreText->Position->x = ScoreXPosition;
    scoreText->Position->y = ScoreYPosition;

    pressR = drawController.Register(new Image(pressRTexture, new Vector2()));
    pressR->Position->x = SCREEN_WIDTH / 2 - pressR->GetWidth() / 2;
    pressR->Active = false;

    SetScore(0);
}

void RestartGame() {
    lastReusedTube = nullptr;

    for (int i = 0; i < tubes.size(); ++i)
        SetNewTubesPairXPosition(i);

    bird->SetVelocity(0, 0);
    bird->Position->x = BirdXPosition;
    bird->Position->y = 0;

    nearestTubesPairIndex = 0;

    SetScore(0);

    birdSpeed = StartBirdSpeed;

    gameState = EGameState::Game;
    pressR->Active = false;
}


void HandleInput() {
    if (is_key_pressed(VK_ESCAPE))
        schedule_quit_game();

    if (is_key_pressed(VK_SPACE)) {
        bird->SetVelocity(0, -JumpVelocity);
    }

    if (is_key_pressed(0x52)) { // R key
        if (gameState == EGameState::Pause) {
            gameState = EGameState::Game;
            pressR->Active = false;
        }
        else if (gameState == EGameState::Lose) {
            RestartGame();
        }
    }
} 

void MoveTubes(float dt) {
    for (int i = 0; i < tubes.size(); ++i) {
        tubes[i]->MoveXAxis(-dt * birdSpeed);

        
        if (tubes[i]->RightXPosition() < 0) {
            SetNewTubesPairXPosition(i);
        }
    }
}

void CheckPassNearestTube() {
    if (bird->Position->x > tubes[nearestTubesPairIndex]->RightXPosition()) {
         nearestTubesPairIndex = (nearestTubesPairIndex + 1) % tubes.size();
         SetScore(score + 1);
    }
}

bool CheckCollisionWithNearestTube() {
    if (bird->CheckCollision(tubes[nearestTubesPairIndex]->bottomTube) ||
        bird->CheckCollision(tubes[nearestTubesPairIndex]->topTube)) {

        return true;
    }

    return false;
}

bool CheckScreenBoundaries() {
    std::pair<Vector2, Vector2> collisionBox = bird->GetCollisionBox();
    return collisionBox.first.y < 0 || collisionBox.second.y > SCREEN_HEIGHT;
}

// this function is called to update game data,
// dt - time elapsed since the previous update (in seconds)
void act(float dt)
{
    HandleInput();

    if (gameState == EGameState::Game) {
        physics.Update(dt);

        birdSpeed += IncreaseBirdSpeedCoeff * dt;

        MoveTubes(dt);

        CheckPassNearestTube();
        
        if (CheckCollisionWithNearestTube() || CheckScreenBoundaries()) {
            gameState = EGameState::Lose;
            pressR->Active = true;
        }

        if (!is_window_active()) {
            gameState = EGameState::Pause;
            pressR->Active = true;
        }
    }
}

// fill buffer in this function
// uint32_t buffer[SCREEN_HEIGHT][SCREEN_WIDTH] - is an array of 32-bit colors (8 bits per R, G, B)
void draw()
{
    // clear backbuffer
    memset(buffer, 0, SCREEN_HEIGHT * SCREEN_WIDTH * sizeof(uint32_t));
    drawController.Draw(buffer);
}

// free game data in this function
void finalize()
{   
    delete bird;
    delete background;
    delete scoreText;

    for (int i = 0; i < tubes.size(); ++i) delete tubes[i];

    delete backgroundTexture;
    delete tubeTexture;
    delete birdTexture;
    delete fontTexture;
    delete pressRTexture;
}

