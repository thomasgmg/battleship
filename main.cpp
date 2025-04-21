#include "raylib.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <thread>

#include "client.h"
#include "fleet.h"
#include "game.h"

using namespace std;

Font font;
int const MAX_POSITIONING_ATTEMPTS = 100;

// Grid
int const GRID_HORIZONTAL_SIZE = 10;
int const GRID_VERTICAL_SIZE = 10;
int const BLOCK_SIZE = 50;
int grid[GRID_VERTICAL_SIZE][GRID_HORIZONTAL_SIZE] = {0};
int opponentGrid[GRID_VERTICAL_SIZE][GRID_HORIZONTAL_SIZE] = {0};
int gridWidth = GRID_HORIZONTAL_SIZE * BLOCK_SIZE;
int gridHeight = GRID_VERTICAL_SIZE * BLOCK_SIZE;
bool showGrid = true;

int const screenWidth = 1250;
int const screenHeight = 700;

// Attacking related declarations
int attackCount = 0;
int const MAX_ATTACKS = 3;

bool showGlow = false;
int glowGridX = -1;
int glowGridY = -1;
float glowTimer = 0.0f;
float const GLOW_DURATION = 1.5f;

// Themes
bool isDarkBackground = true;

const int GRID_OFFSET_X = (screenWidth - GRID_HORIZONTAL_SIZE * BLOCK_SIZE) / 2 - 300;
const int GRID_OFFSET_Y = (screenHeight - GRID_VERTICAL_SIZE * BLOCK_SIZE) / 2;
const int GRID_OPPONENT_OFFSET_X = (screenWidth - GRID_HORIZONTAL_SIZE * BLOCK_SIZE) / 2 + 300;
const int GRID_OPPONENT_OFFSET_Y = (screenHeight - GRID_VERTICAL_SIZE * BLOCK_SIZE) / 2;

float gameTime = 0.0f;

// Background textures
Texture2D seaBackground;
Texture2D battleshipBackground;

// Music / Sound
Music backgroundMusic;
bool isMusicPlaying = false;

// Declarations
void UpdateGame(void);
void DrawGrid(void);
void DrawOpponentGrid(void);
void DrawCoordinates(void);
void DrawGame(float gameTime);
void UnloadGame(void);
void DrawShip(Ship ship);
bool canPlaceShip(Ship ship);
void placeShip(Ship ship);
void distributeFleet(void);
void initFleet(void);
void resetGrid(void);
void getAttackCoordinates(void);
void showClickedGridBlock(void);

int main(int argc, char *argv[])
{
    string ip = "127.0.0.1";
    int port = 8000;

    if (argc > 1)
    {
        ip = argv[1];
    }
    if (argc > 2)
    {
        port = atoi(argv[2]);
    }
    // connectToServer(ip, port);

    SetConfigFlags(FLAG_WINDOW_TRANSPARENT);

    srand(time(0));
    InitWindow(screenWidth, screenHeight, "Classic Game: BATTLESHIP");

    seaBackground = LoadTexture("resources/seeaBackground.jpeg");
    battleshipBackground = LoadTexture("resources/battleshipBackground.jpeg");

    InitAudioDevice();
    backgroundMusic = LoadMusicStream("resources/Battleship_music.mp3");

    SetTargetFPS(60);

    font = LoadFontEx("resources/font.ttf", 96, 0, 0);

    initFleet();
    while (!WindowShouldClose())
    {
        UpdateMusicStream(backgroundMusic);

        gameTime += GetFrameTime();

        Game &game = getGame();
        switch (game.gameState)
        {
        case HOME: {
            if (IsKeyPressed(KEY_SPACE))
            {
                game.gameState = NEW_GAME;
            }
            break;
        }

        case NEW_GAME: {
            disconnectFromServer();
            connectToServer(ip, port);

            resetGrid();

            string playerCode = sendHello();
            getPlayer().code = playerCode;

            distributeFleet();

            game.gameState = SENDING_FLEET;
            // sendFleet(getFleet());
            thread t(sendFleet, ref(getFleet()));
            t.detach();

            // // TODO do this only when server sends START ...
            // game.gameState = PLAYING;
            // PlayMusicStream(backgroundMusic);
            // isMusicPlaying = true;
            break;
        }

        case SENDING_FLEET: {
            break;
        }

        case WAITING_FOR_TURN: {
            break;
        }

        case ATTACKING: {
            getAttackCoordinates();
            break;
        }

        case PLAYING: {
            if (IsKeyPressed(KEY_H))
            {
                game.gameState = HOME;
            }
            break;
        }
        }
        UpdateGame();
        DrawGame(gameTime);
    }

    disconnectFromServer();
    UnloadGame();
    CloseWindow();
    return 0;
}

void getAttackCoordinates(void)
{
    if (attackCount >= MAX_ATTACKS)
    {
        return;
    }

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        Vector2 mousePoint = GetMousePosition();

        // Define the opponent's grid rectangle
        Rectangle opponentGridRect = {(float)GRID_OPPONENT_OFFSET_X, (float)GRID_OPPONENT_OFFSET_Y,
                                      (float)(GRID_HORIZONTAL_SIZE * BLOCK_SIZE),
                                      (float)(GRID_VERTICAL_SIZE * BLOCK_SIZE)};

        if (CheckCollisionPointRec(mousePoint, opponentGridRect))
        {
            int gridX = (int)((mousePoint.x - GRID_OPPONENT_OFFSET_X) / BLOCK_SIZE);
            int gridY = (int)((mousePoint.y - GRID_OPPONENT_OFFSET_Y) / BLOCK_SIZE);

            if (gridX >= 0 && gridX < GRID_HORIZONTAL_SIZE && gridY >= 0 && gridY < GRID_VERTICAL_SIZE)
            {
                attackCount++;
                //  print coordinates (temporary)
                printf("Attack coordinates: (%d, %d)\n", gridX, gridY);
                // Example: sendAttack(gridX, gridY);

                showGlow = true;
                glowGridX = gridX;
                glowGridY = gridY;
                glowTimer = 0.0f;
            }
        }
    }
}

void showClickedGridBlock(void)
{
    if (showGlow && glowGridX >= 0 && glowGridX < GRID_HORIZONTAL_SIZE && glowGridY >= 0 &&
        glowGridY < GRID_VERTICAL_SIZE)
    {
        int glowX = GRID_OPPONENT_OFFSET_X + glowGridX * BLOCK_SIZE;
        int glowY = GRID_OPPONENT_OFFSET_Y + glowGridY * BLOCK_SIZE;
        DrawRectangle(glowX, glowY, BLOCK_SIZE, BLOCK_SIZE, (Color){255, 255, 0, 100});
    }
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

void distributeFleet(void)
{
    Fleet &fleet = getFleet();

    bool success = false;

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
                success = true;
                break;
            }
            else
            {
                // printf("could not place carrier, trying again...\n");
            }
        }
        if (!success)
        {
            printf("Attempts to place submarine exausted, exiting game...\n");
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
                success = true;
                break;
            }
            else
            {
                // printf("could not place cruiser, trying again...\n");
            }
        }
        if (!success)
        {
            printf("Attempts to place submarine exausted, exiting game...\n");
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
                success = true;
                break;
            }
            else
            {
                // printf("could not place battleship, trying again...\n");
            }
        }
        if (!success)
        {
            printf("Attempts to place submarine exausted, exiting game...\n");
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
                success = true;
                break;
            }
            else
            {
                // printf("could not place destroyer, trying again...\n");
            }
        }
        if (!success)
        {
            printf("Attempts to place submarine exausted, exiting game...\n");
        }
    }
    // Submarine
    for (int i = 0; i < 4; ++i)
    {
        for (int j = 0; j < MAX_POSITIONING_ATTEMPTS; ++j)
        {
            Ship &ship = fleet.submarines[i];
            seedShip(ship);

            if (canPlaceShip(ship))
            {
                placeShip(ship);
                success = true;
                break;
            }
            else
            {
                // printf("could not place submarine, trying again...\n");
            }
        }

        if (!success)
        {
            printf("Attempts to place submarine exausted, exiting game...\n");
        }
    }
}

void DrawFleet(void)
{
    Fleet const &fleet = getFleet();

    for (int i = 0; i < NUMBER_OF_CARRIERS; i++)
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

void DrawShip(Ship ship)
{
    for (int i = 0; i < ship.size; i++)
    {
        int positionX = GRID_OFFSET_X + (int)(ship.positions[i].x * BLOCK_SIZE);
        int positionY = GRID_OFFSET_Y + (int)(ship.positions[i].y * BLOCK_SIZE);

        DrawRectangleRounded((Rectangle){(float)positionX, (float)positionY, (float)BLOCK_SIZE, (float)BLOCK_SIZE},
                             0.3f, 8, (Color)Fade({180, 180, 180, 255}, 0.5));

        DrawRectangle(positionX + BLOCK_SIZE / 4, positionY + BLOCK_SIZE / 4, BLOCK_SIZE / 2, BLOCK_SIZE / 2, DARKGRAY);
    }
}

// Grids
void DrawCoordinates(int offsetX, int offsetY)
{
    // Coordinates a - j
    float coordFontSize = 25.0f;
    for (int x = 0; x < GRID_HORIZONTAL_SIZE; x++)
    {
        char letter[2] = {(char)('A' + x), '\0'};
        Vector2 textSize = MeasureTextEx(font, letter, coordFontSize, 1);
        int textX = offsetX + x * BLOCK_SIZE + (BLOCK_SIZE - (int)textSize.x) / 2;
        int textY = offsetY - (int)textSize.y - 5;
        DrawTextEx(font, letter, (Vector2){(float)textX, (float)textY}, coordFontSize, 1, BLACK);
    }
    // Coordinates 1 - 10
    for (int y = 0; y < GRID_VERTICAL_SIZE; y++)
    {
        char number[3];
        sprintf(number, "%d", y + 1);
        Vector2 textSize = MeasureTextEx(font, number, coordFontSize, 1);
        int textX = offsetX - (int)textSize.x - 5;
        int textY = offsetY + y * BLOCK_SIZE + (BLOCK_SIZE - (int)textSize.y) / 2;
        DrawTextEx(font, number, (Vector2){(float)textX, (float)textY}, coordFontSize, 1, BLACK);
    }
}

void DrawGrid(void)
{
    int gridX = GRID_OFFSET_X;
    int gridY = GRID_OFFSET_Y;

    // Draw sea as grid background
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

    DrawTextEx(font, "My fleet:", (Vector2){(float)GetScreenWidth() / 2 - 375, (float)GetScreenHeight() / 2 - 330}, 35,
               1, BLACK);

    DrawCoordinates(GRID_OFFSET_X, GRID_OFFSET_Y);
}

void DrawOpponentGrid(void)
{
    int gridX = GRID_OPPONENT_OFFSET_X;
    int gridY = GRID_OPPONENT_OFFSET_Y;

    // draw sea as grid background
    DrawTexturePro(seaBackground, {0, 0, (float)seaBackground.width, (float)seaBackground.height},
                   {(float)gridX, (float)gridY, (float)gridWidth, (float)gridHeight}, {0, 0}, 0.0f, WHITE);

    if (showGrid)
    {
        for (int y = 0; y < GRID_VERTICAL_SIZE; y++)
        {
            for (int x = 0; x < GRID_HORIZONTAL_SIZE; x++)
            {
                int screenX = GRID_OPPONENT_OFFSET_X + x * BLOCK_SIZE;
                int screenY = GRID_OPPONENT_OFFSET_Y + y * BLOCK_SIZE;

                DrawRectangleLines(screenX, screenY, BLOCK_SIZE, BLOCK_SIZE, BLACK);
            }
        }
    }
    DrawRectangleLines(gridX, gridY, gridWidth, gridHeight, BLACK);

    DrawTextEx(font,
               "Opponent's fleet:", (Vector2){(float)GetScreenWidth() / 2 + 180, (float)GetScreenHeight() / 2 - 330},
               35, 1, BLACK);

    DrawCoordinates(GRID_OPPONENT_OFFSET_X, GRID_OPPONENT_OFFSET_Y);

    showClickedGridBlock();
}

void UpdateGame(void)
{
    if (IsKeyPressed(KEY_G))
    {
        showGrid = !showGrid;
    }

    Game &game = getGame();
    if (showGlow)
    {
        glowTimer += GetFrameTime();
        if (glowTimer >= GLOW_DURATION)
        {
            showGlow = false;
        }
    }
}

void DrawGame(float gameTime)
{
    BeginDrawing();

    Game &game = getGame();
    switch (game.gameState)
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

    case NEW_GAME: {
    }

    case PLAYING: {
        ClearBackground(BLUE);
        DrawGrid();
        DrawOpponentGrid();
        DrawFleet();
        break;
    }
    }
    EndDrawing();
}

void UnloadGame(void)
{
    // for (int i = 0; i < 1; i++)
    //     delete[] fleet.carriers[i].positions;
    // for (int i = 0; i < 1; i++)
    //     delete[] fleet.cruisers[i].positions;
    // for (int i = 0; i < 2; i++)
    //     delete[] fleet.battleships[i].positions;
    // for (int i = 0; i < 3; i++)
    //     delete[] fleet.destroyers[i].positions;
    // for (int i = 0; i < 4; i++)
    //     delete[] fleet.submarines[i].positions;
    UnloadFont(font);
    UnloadTexture(seaBackground);
    UnloadTexture(battleshipBackground);
}
