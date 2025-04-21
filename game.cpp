#include "game.h"

Player player;

Game game;

Player &getPlayer(void)
{
    return player;
}

Game &getGame(void)
{
    return game;
}
