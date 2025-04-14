#include "raylib.h"
#include <cstdlib>
#include <ctime>

Font font;

// Grid
int const GRID_HORIZONTAL_SIZE = 10;
int const GRID_VERTICAL_SIZE = 10;
int const BLOCK_SIZE = 50;
int grid[GRID_VERTICAL_SIZE][GRID_HORIZONTAL_SIZE] = {0};
int gridWidth = GRID_HORIZONTAL_SIZE * BLOCK_SIZE;
int gridHeight = GRID_VERTICAL_SIZE * BLOCK_SIZE;
bool showGrid = true;

int const screenWidth = 1150;
int const screenHeight = 650;

// Themes
bool isDarkBackground = true;

const int GRID_OFFSET_X = (screenWidth - GRID_HORIZONTAL_SIZE * BLOCK_SIZE) / 2;
const int GRID_OFFSET_Y = (screenHeight - GRID_VERTICAL_SIZE * BLOCK_SIZE) / 2;

// Background textures
Texture2D seaBackground;

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
    ShipType shipType;
    int size;
    bool isHorizontal;
    Vector2 positions[];
};
struct Fleet
{
    Ship carriers[5];
    Ship battleship[4];
    Ship cruiser[3];
    Ship destroyer[2];
    Ship submarine[1];
};

// Declarations
void UpdateGame(void);
void DrawGrid(void);
void DrawGame(void);
void UnloadGame(void);

int main(void)
{
    SetConfigFlags(FLAG_WINDOW_TRANSPARENT);

    srand(time(0));
    InitWindow(screenWidth, screenHeight, "Classic Game: BATTLESHIP");

    SetTargetFPS(60);

    seaBackground = LoadTexture("resources/seaBackground.jpeg");
    font = LoadFontEx("resources/font.ttf", 96, 0, 0);

    while (!WindowShouldClose())
    {
        switch (gameState)
        {
        case HOME: {
            if (IsKeyPressed(KEY_SPACE))
            {
                gameState = PLAYING;
            }
        }

        case PLAYING: {
            if (IsKeyPressed(KEY_H))
            {
                gameState = HOME;
            }
        }
        }
        UpdateGame();
    }
    UnloadGame();
    CloseWindow();
    return 0;
}

// Ships
Ship newCarrier(void)
{
}
Ship newBattleship(void)
{
}
Ship newCruiser(void)
{
}
Ship newDestroyer(void)
{
}
Ship newSubmarine(void)
{
}

// Grid
void DrawGrid(void)
{
    int gridX = GRID_OFFSET_X;
    int gridY = GRID_OFFSET_Y;

    DrawRectangle(gridX, gridY, gridWidth, gridHeight, isDarkBackground ? DARKBLUE : BLUE);

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
}

void UpdateGame(void)
{
    if (IsKeyPressed(KEY_T))
    {
        isDarkBackground = !isDarkBackground;
    }
    if (IsKeyPressed(KEY_G))
    {
        showGrid = !showGrid;
    }
}

void DrawGame(void)
{
    BeginDrawing();
    switch (gameState)
    {
    case HOME: {
        ClearBackground(BLACK);
    }

    case PLAYING: {
        ClearBackground(Fade(BLUE, 0.1f));
        DrawGrid();
    }
    }
    EndDrawing();
}

void UnloadGame(void)
{
    UnloadFont(font);
    UnloadTexture(seaBackground);
}
