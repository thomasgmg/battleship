#include "raylib.h"

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

Fleet &getFleet();
void initFleet(void);
void seedShip(Ship &ship);
void buildCarrier(Ship &ship);
void buildCruiser(Ship &ship);
void buildBattleship(Ship &ship);
void buildDestroyer(Ship &ship);
