#ifndef GAME_H
#define GAME_H

#include <string>

using namespace std;

// Structs and Types
enum GameState
{
    HOME,
    NEW_GAME,
    WAITING_FOR_TURN,
    ATTACKING,
    SENDING_FLEET,
    PLAYING
};

struct Player
{
    string name;
    string code;
};

struct Game
{
    GameState gameState = HOME;
};

Player &getPlayer(void);

Game &getGame(void);

#endif
