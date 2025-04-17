#include "fleet.h"
#include <cstdio>

Fleet fleet;

int const MAX_POSITIONING_ATTEMPTS = 100;

Fleet &getFleet()
{
    return fleet;
}

// Ship initialization
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
