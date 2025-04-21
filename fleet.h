#ifndef FLEET_H
#define FLEET_H

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

const int NUMBER_OF_CARRIERS = 1;
const int NUMBER_OF_CRUISERS = 1;
const int NUMBER_OF_BATTLESHIPS = 2;
const int NUMBER_OF_DESTROYERS = 3;
const int NUMBER_OF_SUBMARINES = 4;

Fleet &getFleet();
void initFleet(void);
void seedShip(Ship &ship);
void buildCarrier(Ship &ship);
void buildCruiser(Ship &ship);
void buildBattleship(Ship &ship);
void buildDestroyer(Ship &ship);

#endif
