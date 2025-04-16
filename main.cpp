#include "raylib.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>

// Structs and Types
enum GameState
{
    HOME,
    NEW_GAME,
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
int const MAX_POSITIONING_ATTEMPTS = 100;

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
void distributeFleet(void);
void initFleet(void);
void resetGrid(void);

int main(void)
{
    SetConfigFlags(FLAG_WINDOW_TRANSPARENT);

    srand(time(0));
    InitWindow(screenWidth, screenHeight, "Classic Game: BATTLESHIP");

    SetTargetFPS(60);

    seaBackground = LoadTexture("resources/seeaBackground.jpeg");
    battleshipBackground = LoadTexture("resources/battleshipBackground.jpeg");

    font = LoadFontEx("resources/font.ttf", 96, 0, 0);

    initFleet();
    while (!WindowShouldClose())
    {
        gameTime += GetFrameTime();

        switch (gameState)
        {
        case HOME: {
            if (IsKeyPressed(KEY_SPACE))
            {
                gameState = NEW_GAME;
            }
            break;
        }

        case NEW_GAME: {
            resetGrid();
            distributeFleet();
            gameState = PLAYING;
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

void resetGrid(void)
{
    // Clear grid
    for (int y = 0; y < GRID_VERTICAL_SIZE; y++)
    {
        for (int x = 0; x < GRID_HORIZONTAL_SIZE; x++)
        {
            grid[y][x] = 0;
        }
    }
}

// Ships functions
bool canPlaceShip(Ship ship)
{
    // if (true)
    // {
    //     return true;
    // }

    // Check bounds and occupancy
    for (int i = 0; i < ship.size; i++)
    {
        int x = (int)ship.positions[i].x;
        int y = (int)ship.positions[i].y;
        if (x < 0 || x >= GRID_HORIZONTAL_SIZE || y < 0 || y >= GRID_VERTICAL_SIZE)
        {
            return false;
        }
    }

    for (int i = 0; i < ship.size; i++)
    {
        int x = (int)ship.positions[i].x;
        int y = (int)ship.positions[i].y;

        for (int dy = -1; dy <= 1; dy++)
        {
            for (int dx = -1; dx <= 1; dx++)
            {
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

Ship initCarriers(void)
{
    Ship ship;
    ship.size = 5;
    ship.type = CARRIER;
    ship.positions = new Vector2[5];

    return ship;
}
Ship initCruisers(void)
{
    Ship ship;
    ship.size = 4;
    ship.type = CRUISER;
    ship.isHorizontal = GetRandomValue(0, 1);
    ship.positions = new Vector2[4];

    return ship;
}
Ship initBattleships(void)
{
    Ship ship;
    ship.size = 3;
    ship.type = BATTLESHIP;
    ship.positions = new Vector2[3];

    return ship;
}
Ship initDestroyers(void)
{
    Ship ship;
    ship.size = 2;
    ship.type = DESTROYER;
    ship.positions = new Vector2[2];

    return ship;
}
Ship initSubmarines(void)
{
    Ship ship;
    ship.size = 1;
    ship.type = SUBMARINE;
    ship.positions = new Vector2[1];

    return ship;
}

void initFleet(void)
{
    int numberOfShips = 1;
    for (int i = 0; i < numberOfShips; i++)
    {
        fleet.carriers[i] = initCarriers();
    }

    numberOfShips = 1;
    for (int i = 0; i < numberOfShips; i++)
    {
        fleet.cruisers[i] = initCruisers();
    }

    numberOfShips = 2;
    for (int i = 0; i < numberOfShips; i++)
    {
        fleet.battleships[i] = initBattleships();
    }

    numberOfShips = 3;
    for (int i = 0; i < numberOfShips; i++)
    {
        fleet.destroyers[i] = initDestroyers();
    }

    numberOfShips = 4;
    for (int i = 0; i < numberOfShips; i++)
    {
        fleet.submarines[i] = initSubmarines();
    }
}

void seedShip(Ship &ship)
{
    ship.isHorizontal = GetRandomValue(0, 1);

    float x = (float)GetRandomValue(0, 9);
    float y = (float)GetRandomValue(0, 9);
    ship.positions[0] = (Vector2){x, y};
}

void buildCarrier(Ship &ship)
{
    float x = ship.positions[0].x;
    float y = ship.positions[0].y;

    if (ship.isHorizontal)
    {
        ship.positions[1] = (Vector2){x + 1, y};
        ship.positions[2] = (Vector2){x + 2, y};
        ship.positions[3] = (Vector2){x + 2, y - 1};
        ship.positions[4] = (Vector2){x + 2, y + 1};
    }
    else
    {
        ship.positions[1] = (Vector2){x, y + 1};
        ship.positions[2] = (Vector2){x, y + 2};
        ship.positions[3] = (Vector2){x - 1, y + 2};
        ship.positions[4] = (Vector2){x + 1, y + 2};
    }
}

void buildCruiser(Ship &ship)
{
    float x = ship.positions[0].x;
    float y = ship.positions[0].y;

    if (ship.isHorizontal)
    {
        ship.positions[1] = (Vector2){x + 1, y};
        ship.positions[2] = (Vector2){x + 2, y};
        ship.positions[3] = (Vector2){x + 3, y};
    }
    else
    {
        ship.positions[1] = (Vector2){x, y + 1};
        ship.positions[2] = (Vector2){x, y + 2};
        ship.positions[3] = (Vector2){x, y + 3};
    }
}

void buildBattleship(Ship &ship)
{
    float x = ship.positions[0].x;
    float y = ship.positions[0].y;

    if (ship.isHorizontal)
    {
        ship.positions[1] = (Vector2){x + 1, y};
        ship.positions[2] = (Vector2){x + 2, y};
    }
    else
    {
        ship.positions[1] = (Vector2){x, y + 1};
        ship.positions[2] = (Vector2){x, y + 2};
    }
}

void buildDestroyer(Ship &ship)
{
    float x = ship.positions[0].x;
    float y = ship.positions[0].y;

    if (ship.isHorizontal)
    {
        ship.positions[1] = (Vector2){x + 1, y};
    }
    else
    {
        ship.positions[1] = (Vector2){x, y + 1};
    }
}

void distributeFleet(void)
{
    // Carrier
    for (int i = 0; i != 1; ++i)
    {
        for (int j = 0; j != MAX_POSITIONING_ATTEMPTS; ++j)
        {
            Ship &ship = fleet.carriers[i];
            seedShip(ship);
            buildCarrier(ship);

            if (canPlaceShip(ship))
            {
                placeShip(ship);
                break;
            }
            else
            {
                // printf("could not place carrier, trying again...\n");
            }
        }
    }
    // Cruiser
    for (int i = 0; i != 1; ++i)
    {
        for (int j = 0; j != MAX_POSITIONING_ATTEMPTS; ++j)
        {
            Ship &ship = fleet.cruisers[i];
            seedShip(ship);
            buildCruiser(ship);

            if (canPlaceShip(ship))
            {
                placeShip(ship);
                break;
            }
            else
            {
                // printf("could not place cruiser, trying again...\n");
            }
        }
    }
    // Battleship
    for (int i = 0; i < 2; ++i)
    {
        for (int j = 0; j != MAX_POSITIONING_ATTEMPTS; ++j)
        {
            Ship &ship = fleet.battleships[i];
            seedShip(ship);
            buildBattleship(ship);

            if (canPlaceShip(ship))
            {
                placeShip(ship);
                break;
            }
            else
            {
                // printf("could not place battleship, trying again...\n");
            }
        }
    }
    // Destroyer
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j != MAX_POSITIONING_ATTEMPTS; ++j)
        {
            Ship &ship = fleet.destroyers[i];
            seedShip(ship);
            buildDestroyer(ship);

            if (canPlaceShip(ship))
            {
                placeShip(ship);
                break;
            }
            else
            {
                // printf("could not place destroyer, trying again...\n");
            }
        }
    }
    // Submarine
    for (int i = 0; i < 4; ++i)
    {
        bool success = false;
        for (int j = 0; j < MAX_POSITIONING_ATTEMPTS; ++j)
        {
            Ship &ship = fleet.submarines[i];
            seedShip(ship);

            if (canPlaceShip(ship))
            {
                placeShip(ship);
                success = true;
                // printf("placed submarine succesfully\n");
                break;
            }
            else
            {
                // printf("could not place submarine, trying again...\n");
            }
        }

        if (!success)
        {
            // printf("Attempts to place submarine exausted, exiting game...\n");
        }
    }

    printf("distributeFleet - end\n");
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
    for (int i = 0; i < 2; i++)
    {
        DrawShip(fleet.battleships[i]);
    }
    for (int i = 0; i < 3; i++)
    {
        DrawShip(fleet.destroyers[i]);
    }
    for (int i = 0; i < 4; i++)
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
    // if (IsKeyDown(KEY_N) && IsKeyPressed(KEY_F))
    // {
    //     printf("Freezing\n");
    //     newFleet();
    // }
}

void DrawShip(Ship ship)
{
    for (int i = 0; i < ship.size; i++)
    {
        // int positionX = GRID_OFFSET_X + (int)(ship.positions[i].x * BLOCK_SIZE) + BLOCK_SIZE / 4;
        // int positionY = GRID_OFFSET_Y + (int)(ship.positions[i].y * BLOCK_SIZE) + BLOCK_SIZE / 4;
        int positionX = GRID_OFFSET_X + (int)(ship.positions[i].x * BLOCK_SIZE);
        int positionY = GRID_OFFSET_Y + (int)(ship.positions[i].y * BLOCK_SIZE);

        DrawRectangle(positionX, positionY, BLOCK_SIZE, BLOCK_SIZE, (Color)Fade({180, 180, 180, 255}, 0.7));
        // DrawRectangleRounded();
        DrawRectangle(positionX + BLOCK_SIZE / 4, positionY + BLOCK_SIZE / 4, BLOCK_SIZE / 2, BLOCK_SIZE / 2, DARKGRAY);
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
    for (int i = 0; i < 1; i++)
        delete[] fleet.carriers[i].positions;
    for (int i = 0; i < 1; i++)
        delete[] fleet.cruisers[i].positions;
    for (int i = 0; i < 2; i++)
        delete[] fleet.battleships[i].positions;
    for (int i = 0; i < 3; i++)
        delete[] fleet.destroyers[i].positions;
    for (int i = 0; i < 4; i++)
        delete[] fleet.submarines[i].positions;
    UnloadFont(font);
    UnloadTexture(seaBackground);
    UnloadTexture(battleshipBackground);
}
