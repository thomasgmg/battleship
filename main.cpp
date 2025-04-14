#include "raylib.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>

// Structs and Types
enum GameState
{
    HOME,
    PLAYING
};
GameState gameState = HOME;

enum ShipType
{
    CARRIER,
    BATTLESHIP,
    CRUISER,
    DESTROYER,
    SUBMARINE
};
struct Ship
{
    ShipType type;
    int size;
    bool isHorizontal;
    Vector2 *positions;
};
struct Fleet
{
    Ship carriers[5];
    Ship battleships[4];
    Ship cruisers[3];
    Ship destroyers[2];
    Ship submarines[1];
};
Fleet fleet;

Font font;

// Grid
int const GRID_HORIZONTAL_SIZE = 10;
int const GRID_VERTICAL_SIZE = 10;
int const BLOCK_SIZE = 60;
int grid[GRID_VERTICAL_SIZE][GRID_HORIZONTAL_SIZE] = {0};
int gridWidth = GRID_HORIZONTAL_SIZE * BLOCK_SIZE;
int gridHeight = GRID_VERTICAL_SIZE * BLOCK_SIZE;
bool showGrid = true;

int const screenWidth = 1250;
int const screenHeight = 700;

// Themes
bool isDarkBackground = true;

const int GRID_OFFSET_X = (screenWidth - GRID_HORIZONTAL_SIZE * BLOCK_SIZE) / 2;
const int GRID_OFFSET_Y = (screenHeight - GRID_VERTICAL_SIZE * BLOCK_SIZE) / 2;

float gameTime = 0.0f;

// Background textures
Texture2D seaBackground;
Texture2D battleshipBackground;
// Declarations
void UpdateGame(void);
void DrawGrid(void);
void DrawGame(float gameTime);
void UnloadGame(void);

int main(void)
{
    SetConfigFlags(FLAG_WINDOW_TRANSPARENT);

    srand(time(0));
    InitWindow(screenWidth, screenHeight, "Classic Game: BATTLESHIP");

    SetTargetFPS(60);

    seaBackground = LoadTexture("resources/seeaBackground.jpeg");
    battleshipBackground = LoadTexture("resources/battleshipBackground.jpeg");

    font = LoadFontEx("resources/font.ttf", 96, 0, 0);

    while (!WindowShouldClose())
    {
        gameTime += GetFrameTime();

        switch (gameState)
        {
        case HOME: {
            if (IsKeyPressed(KEY_SPACE))
            {
                gameState = PLAYING;
            }
            break;
        }

        case PLAYING: {
            if (IsKeyPressed(KEY_H))
            {
                gameState = HOME;
            }
            break;
        }
        }
        UpdateGame();
        DrawGame(gameTime);
    }
    UnloadGame();
    CloseWindow();
    return 0;
}

// Ships
Ship newCarrier(void)
{
    Ship ship;
    ship.size = 5;
    ship.type = CARRIER;
    ship.isHorizontal = true;

    // pa = position array
    Vector2 pa[5];
    pa[0] = (Vector2){(float)GetRandomValue(0, 10)};
    pa[1] = (Vector2){};
    pa[2] = (Vector2){};
    pa[3] = (Vector2){};
    pa[4] = (Vector2){};

    ship.positions = pa;

    return ship;
}
Ship newBattleship(void)
{
    Ship ship;
    ship.size = 4;
    ship.type = BATTLESHIP;
    ship.isHorizontal = true;

    Vector2 pa[4];
    pa[0] = (Vector2){(float)GetRandomValue(0, 10)};
    pa[1] = (Vector2){};
    pa[2] = (Vector2){};
    pa[3] = (Vector2){};

    ship.positions = pa;

    return ship;
}
Ship newCruiser(void)
{
    Ship ship;
    ship.size = 3;
    ship.type = CRUISER;
    ship.isHorizontal = true;

    Vector2 pa[3];
    pa[0] = (Vector2){(float)GetRandomValue(0, 10)};
    pa[1] = (Vector2){};
    pa[2] = (Vector2){};

    ship.positions = pa;

    return ship;
}
Ship newDestroyer(void)
{
    Ship ship;
    ship.size = 2;
    ship.type = DESTROYER;
    ship.isHorizontal = true;

    Vector2 *pa = new Vector2[2];
    // Vector2 pa[2];
    float x = (float)GetRandomValue(0, 10);
    float y = (float)GetRandomValue(0, 10);

    pa[0] = (Vector2){x, y};
    pa[1] = (Vector2){x, y - 1};

    ship.positions = pa;

    return ship;
}
Ship newSubmarine(void)
{
    Ship ship;
    ship.size = 1;
    ship.type = SUBMARINE;
    ship.isHorizontal = true;

    Vector2 pa[1];
    pa[0] = (Vector2){(float)GetRandomValue(0, 10)};

    ship.positions = pa;

    return ship;
}
void newFleet(void)
{
    Ship destroyer = newDestroyer();
    fleet.destroyers[0] = destroyer;
}

void UpdateGame(void)
{
    // if (IsKeyPressed(KEY_T))
    // {
    //     isDarkBackground = !isDarkBackground;
    // }
    if (IsKeyPressed(KEY_G))
    {
        showGrid = !showGrid;
    }
    if (gameState == HOME)
    {
        newFleet();
    }
}

void DrawShip(Ship ship)
{
    for (int i = 0; i < ship.size; i++)
    {
        int positionX = GRID_OFFSET_X + ship.positions[i].x * BLOCK_SIZE;
        int positionY = GRID_OFFSET_Y + ship.positions[i].y * BLOCK_SIZE;

        DrawRectangle(positionX, positionY, BLOCK_SIZE, BLOCK_SIZE, WHITE);
    }
}

// Grid
void DrawGrid(void)
{
    int gridX = GRID_OFFSET_X;
    int gridY = GRID_OFFSET_Y;

    // DrawRectangle(gridX, gridY, gridWidth, gridHeight, isDarkBackground ? DARKBLUE : BLUE);

    DrawTexturePro(
        seaBackground,
        {0, 0, (float)seaBackground.width, (float)seaBackground.height},   // Source rectangle (full texture)
        {(float)gridX, (float)gridY, (float)gridWidth, (float)gridHeight}, // Destination rectangle (grid size)
        {0, 0}, 0.0f, WHITE);

    if (showGrid)
    {
        for (int y = 0; y < GRID_VERTICAL_SIZE; y++)
        {
            for (int x = 0; x < GRID_HORIZONTAL_SIZE; x++)
            {
                int screenX = GRID_OFFSET_X + x * BLOCK_SIZE;
                int screenY = GRID_OFFSET_Y + y * BLOCK_SIZE;

                DrawRectangleLines(screenX, screenY, BLOCK_SIZE, BLOCK_SIZE, BLACK);
            }
        }
    }
    DrawRectangleLines(gridX, gridY, gridWidth, gridHeight, BLACK);

    // Coordinates
    float coordFontSize = 25.0f;
    for (int x = 0; x < GRID_HORIZONTAL_SIZE; x++)
    {
        char letter[2] = {(char)('A' + x), '\0'};
        Vector2 textSize = MeasureTextEx(font, letter, coordFontSize, 1);
        int textX = gridX + x * BLOCK_SIZE + (BLOCK_SIZE - (int)textSize.x) / 2;
        int textY = gridY - (int)textSize.y - 5; // Above the grid
        DrawTextEx(font, letter, (Vector2){(float)textX, (float)textY}, coordFontSize, 1, BLACK);
    }
    for (int y = 0; y < GRID_VERTICAL_SIZE; y++)
    {
        char number[3];
        sprintf(number, "%d", y + 1);
        Vector2 textSize = MeasureTextEx(font, number, coordFontSize, 1);
        int textX = gridX - (int)textSize.x - 5; // Left of the grid
        int textY = gridY + y * BLOCK_SIZE + (BLOCK_SIZE - (int)textSize.y) / 2;
        DrawTextEx(font, number, (Vector2){(float)textX, (float)textY}, coordFontSize, 1, BLACK);
    }
}

void DrawGame(float gameTime)
{
    BeginDrawing();
    switch (gameState)
    {
    case HOME: {
        DrawTexturePro(battleshipBackground,
                       {0, 0, (float)battleshipBackground.width, (float)battleshipBackground.height},
                       {0, 0, (float)screenWidth, (float)screenHeight}, {0, 0}, 0.0f, WHITE);

        Vector2 textPos;
        float glowScale = 1.0f + 0.1f * sinf(gameTime * 2.0f);
        int glowLayers = 3;

        textPos = {(float)screenWidth / 2 - MeasureTextEx(font, "Welcome to BATTLESHIP", 40, 1).x / 2 - 100,
                   (float)screenHeight / 2 - 270};

        for (int i = glowLayers; i >= 1; i--)
        {
            float glowSize = 40 + i * 5 * glowScale;
            float glowAlpha = 0.3f - (i * 0.1f);
            Color glowColor = {255, 255, 0, (unsigned char)(glowAlpha * 255)};
            Vector2 glowPos = {(float)screenWidth / 2 -
                                   MeasureTextEx(font, "Welcome to BATTLESHIP", glowSize, 1).x / 2 - 100,
                               (float)screenHeight / 2 - 270 - i * 2};
            DrawTextEx(font, "Welcome to BATTLESHIP", glowPos, glowSize, 1, glowColor);
        }
        DrawTextEx(font, "Welcome to BATTLESHIP", textPos, 40, 1, BLACK);
        break;
    }

    case PLAYING: {
        // ClearBackground(Fade(BLUE, 0.1f));
        ClearBackground(BLUE);
        DrawGrid();
        DrawShip(fleet.destroyers[0]);
        break;
    }
    }
    EndDrawing();
}

void UnloadGame(void)
{
    UnloadFont(font);
    UnloadTexture(seaBackground);
    UnloadTexture(battleshipBackground);
}
