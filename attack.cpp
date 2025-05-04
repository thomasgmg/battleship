#include "attack.h"
#include "game.h"
#include "raylib.h"

#include <cstdio>

int const GRID_HORIZONTAL_SIZE = 10;
int const GRID_VERTICAL_SIZE = 10;
int const BLOCK_SIZE = 50;
const int GRID_OFFSET_X = (1250 - GRID_HORIZONTAL_SIZE * BLOCK_SIZE) / 2 - 300;
const int GRID_OFFSET_Y = (700 - GRID_VERTICAL_SIZE * BLOCK_SIZE) / 2;
const int GRID_OPPONENT_OFFSET_X = (1250 - GRID_HORIZONTAL_SIZE * BLOCK_SIZE) / 2 + 300;
const int GRID_OPPONENT_OFFSET_Y = (700 - GRID_VERTICAL_SIZE * BLOCK_SIZE) / 2;

bool showGlow = false;
int glowGridX = -1;
int glowGridY = -1;
float glowTimer = 0.0f;
float const GLOW_DURATION = 1.5f;

string[] getAttackCoordinates(void)
{
    string attacks[3];
    const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE] = {0};

    int attackCount = 0;
    int const MAX_ATTACKS = 3;


    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
    {
        Vector2 mousePoint = GetMousePosition();

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
                printf("Attack coordinates: (%d, %d)\n", gridX, gridY);
                // Example: sendAttack(gridX, gridY);

                showGlow = true;
                glowGridX = gridX;
                glowGridY = gridY;
                glowTimer = 0.0f;
            }
        }
    }
    showClickedGridBlock();

    return attacks;
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

    if (showGlow)
    {
        glowTimer += GetFrameTime();
        if (glowTimer >= GLOW_DURATION)
        {
            showGlow = false;
        }
    }
}
