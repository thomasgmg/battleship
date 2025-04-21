#ifndef CLIENT_H
#define CLIENT_H

#include <string>

#include "fleet.h"
#include "game.h"

using namespace std;

void connectToServer(string const &, int);
void disconnectFromServer();
string waitForResponse(void);
string sendHello(void);
// string sendMessage(string const &message);
void sendFleet(Fleet const &fleet);
void waitForTurn(void);

#endif
