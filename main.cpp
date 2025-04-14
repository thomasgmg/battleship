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
    CRUISER,
    BATTLESHIP,
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
    Ship carriers[1];
    Ship cruisers[1];
    Ship battleships[2];
    Ship destroyers[3];
    Ship submarines[4];
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
void DrawShip(Ship ship);
bool canPlaceShip(Ship ship);
void placeShip(Ship ship);
void newFleet();

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
                newFleet();
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

// Ships functions
bool canPlaceShip(Ship ship)
{
    for (int i = 0; i < ship.size; i++)
    {
        int x = (int)ship.positions[i].x;
        int y = (int)ship.positions[i].y;
        // Check bounds and occupancy
        if (x < 0 || x >= GRID_HORIZONTAL_SIZE || y < 0 || y >= GRID_VERTICAL_SIZE || grid[y][x] != 0)
        {
            return false;
        }
        for (int dy = -1; dy <= 1; dy++)
        {
            for (int dx = -1; dx <= 1; dx++)
            {
                if (dx == 0 && dy == 0) // Skip the ship's own position
                {
                    continue;
                }
                int nx = x + dx;
                int ny = y + dy;
                // Check if neighbor is within bounds and occupied
                if (nx >= 0 && nx < GRID_HORIZONTAL_SIZE && ny >= 0 && ny < GRID_VERTICAL_SIZE && grid[ny][nx] != 0)
                {
                    return false;
                }
            }
        }
    }
    return true;
}
void placeShip(Ship ship)
{
    for (int i = 0; i < ship.size; i++)
    {
        int x = (int)ship.positions[i].x;
        int y = (int)ship.positions[i].y;
        grid[y][x] = 1;
    }
}

Ship newCarrier(void)
{
    Ship ship;
    ship.size = 5;
    ship.type = CARRIER;
    ship.isHorizontal = GetRandomValue(0, 1);

    Vector2 *pa = new Vector2[5];
    do
    {
        float x, y;
        if (ship.isHorizontal)
        {
            x = (float)GetRandomValue(1, 5);
            y = (float)GetRandomValue(2, 7);
            pa[0] = (Vector2){x, y};
            pa[1] = (Vector2){x + 1, y};
            pa[2] = (Vector2){x + 2, y};
            pa[3] = (Vector2){x + 2, y - 1};
            pa[4] = (Vector2){x + 2, y + 1};
        }
        else
        {
            x = (float)GetRandomValue(2, 7);
            y = (float)GetRandomValue(1, 5);
            pa[0] = (Vector2){x, y};
            pa[1] = (Vector2){x, y + 1};
            pa[2] = (Vector2){x, y + 2};
            pa[3] = (Vector2){x - 1, y + 2};
            pa[4] = (Vector2){x + 1, y + 2};
        }
        ship.positions = pa;
    } while (!canPlaceShip(ship));

    return ship;
}
Ship newCruiser(void)
{
    Ship ship;
    ship.size = 4;
    ship.type = CRUISER;
    ship.isHorizontal = GetRandomValue(0, 1);

    Vector2 *pa = new Vector2[4];
    do
    {
        float x, y;
        if (ship.isHorizontal)
        {
            x = (float)GetRandomValue(1, 8);
            y = (float)GetRandomValue(1, 5);
            pa[0] = (Vector2){x, y};
            pa[1] = (Vector2){x, y + 1};
            pa[2] = (Vector2){x, y + 2};
            pa[3] = (Vector2){x, y + 3};
        }
        else
        {
            x = (float)GetRandomValue(1, 8);
            y = (float)GetRandomValue(1, 5);
            pa[0] = (Vector2){x, y};
            pa[1] = (Vector2){x + 1, y};
            pa[2] = (Vector2){x + 2, y};
            pa[3] = (Vector2){x + 3, y};
        }
        ship.positions = pa;
    } while (!canPlaceShip(ship));

    return ship;
}
Ship newBattleship(void)
{
    Ship ship;
    ship.size = 3;
    ship.type = BATTLESHIP;
    ship.isHorizontal = GetRandomValue(0, 1);

    Vector2 *pa = new Vector2[3];
    do
    {
        float x, y;
        if (ship.isHorizontal)
        {
            x = (float)GetRandomValue(1, 8);
            y = (float)GetRandomValue(3, 8);
            pa[0] = (Vector2){x, y};
            pa[1] = (Vector2){x, y - 1};
            pa[2] = (Vector2){x, y - 2};
        }
        else
        {
            x = (float)GetRandomValue(1, 8);
            y = (float)GetRandomValue(3, 8);
            pa[0] = (Vector2){x, y};
            pa[1] = (Vector2){x - 1, y};
            pa[2] = (Vector2){x - 2, y};
        }
        ship.positions = pa;
    } while (!canPlaceShip(ship));

    return ship;
}
Ship newDestroyer(void)
{
    Ship ship;
    ship.size = 2;
    ship.type = DESTROYER;
    ship.isHorizontal = GetRandomValue(0, 1);

    Vector2 *pa = new Vector2[2];
    do
    {
        float x, y;
        if (ship.isHorizontal)
        {
            x = (float)GetRandomValue(1, 8);
            y = (float)GetRandomValue(2, 8);
            pa[0] = (Vector2){x, y};
            pa[1] = (Vector2){x, y - 1};
        }
        else
        {
            x = (float)GetRandomValue(1, 8);
            y = (float)GetRandomValue(2, 8);
            pa[0] = (Vector2){x, y};
            pa[1] = (Vector2){x + 1, y};
        }
        ship.positions = pa;
    } while (!canPlaceShip(ship));

    return ship;
}
Ship newSubmarine(void)
{
    Ship ship;
    ship.size = 1;
    ship.type = SUBMARINE;
    ship.isHorizontal = true;

    Vector2 *pa = new Vector2[1];
    do
    {
        float x = (float)GetRandomValue(1, 8);
        float y = (float)GetRandomValue(1, 8);
        pa[0] = (Vector2){x, y};
        ship.positions = pa;
    } while (!canPlaceShip(ship));

    return ship;
}

void newFleet(void)
{
    // Clear grid
    for (int y = 0; y < GRID_VERTICAL_SIZE; y++)
    {
        for (int x = 0; x < GRID_HORIZONTAL_SIZE; x++)
        {
            grid[y][x] = 0;
        }
    }

    for (int i = 0; i < 1; i++)
    {
        Ship ship = newCarrier();
        placeShip(ship);
        fleet.carriers[i] = ship;
    }
    for (int i = 0; i < 1; i++)
    {
        Ship ship = newCruiser();
        placeShip(ship);
        fleet.cruisers[i] = ship;
    }
    for (int i = 0; i < 1; i++)
    {
        Ship ship = newBattleship();
        placeShip(ship);
        fleet.battleships[i] = ship;
    }
    for (int i = 0; i < 1; i++)
    {
        Ship ship = newDestroyer();
        placeShip(ship);
        fleet.destroyers[i] = ship;
    }
    for (int i = 0; i < 1; i++)
    {
        Ship ship = newSubmarine();
        placeShip(ship);
        fleet.submarines[i] = ship;
    }
}
void DrawFleet(void)
{
    for (int i = 0; i < 1; i++)
    {
        DrawShip(fleet.carriers[i]);
    }
    for (int i = 0; i < 1; i++)
    {
        DrawShip(fleet.cruisers[i]);
    }
    for (int i = 0; i < 1; i++)
    {
        DrawShip(fleet.battleships[i]);
    }
    for (int i = 0; i < 1; i++)
    {
        DrawShip(fleet.destroyers[i]);
    }
    for (int i = 0; i < 1; i++)
    {
        DrawShip(fleet.submarines[i]);
    }
}

void UpdateGame(void)
{
    if (IsKeyPressed(KEY_G))
    {
        showGrid = !showGrid;
    }
}

void DrawShip(Ship ship)
{
    for (int i = 0; i < ship.size; i++)
    {
        int positionX = GRID_OFFSET_X + (int)(ship.positions[i].x * BLOCK_SIZE) + BLOCK_SIZE / 4;
        int positionY = GRID_OFFSET_Y + (int)(ship.positions[i].y * BLOCK_SIZE) + BLOCK_SIZE / 4;

        DrawRectangle(positionX, positionY, BLOCK_SIZE / 2, BLOCK_SIZE / 2, GRAY);
    }
}

// Grid
void DrawGrid(void)
{
    int gridX = GRID_OFFSET_X;
    int gridY = GRID_OFFSET_Y;

    DrawTexturePro(seaBackground, {0, 0, (float)seaBackground.width, (float)seaBackground.height},
                   {(float)gridX, (float)gridY, (float)gridWidth, (float)gridHeight}, {0, 0}, 0.0f, WHITE);

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
        int textY = gridY - (int)textSize.y - 5;
        DrawTextEx(font, letter, (Vector2){(float)textX, (float)textY}, coordFontSize, 1, BLACK);
    }
    for (int y = 0; y < GRID_VERTICAL_SIZE; y++)
    {
        char number[3];
        sprintf(number, "%d", y + 1);
        Vector2 textSize = MeasureTextEx(font, number, coordFontSize, 1);
        int textX = gridX - (int)textSize.x - 5;
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
        ClearBackground(BLUE);
        DrawGrid();
        DrawFleet();
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
