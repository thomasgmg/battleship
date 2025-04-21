#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <stdio.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#include "client.h"
#include "fleet.h"
#include "game.h"
#include "utils.h"

using namespace std;

int sock = -1;

string waitForResponse(void)
{
    const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE] = {0};

    // Receive server response
    int bytes_received = recv(sock, buffer, BUFFER_SIZE - 1, 0);
    if (bytes_received <= 0)
    {
        perror("Receive failed");
        exit(1);
    }

    // Null-terminate the received message
    buffer[bytes_received] = '\0';
    printf("Server response: %s\n", buffer);

    return string(buffer);
}

string sendMessage(const string &message)
{
    if (sock == -1)
    {
        perror("Socket is not connected\n");
        return "";
    }

    // Send
    printf("Sending message: %s\n", message.c_str());
    if (send(sock, message.c_str(), message.length(), 0) < 0)
    {
        perror("send failed\n");
        exit(1);
    }

    return waitForResponse();
}

string sendHello(void)
{
    string response = sendMessage("HELLO Thomas\n");
    vector<string> words = split(response);

    if (words[0] != "WELCOME")
    {
        perror("error in hello response\n");
        exit(1);
    }

    string playerCode = words[1];

    return playerCode;
}

void sendFleet(Fleet const &fleet)
{
    for (int i = 0; i < NUMBER_OF_CARRIERS; i++)
    {
        Vector2 origin = fleet.carriers[i].positions[0];
        string h = fleet.carriers[i].isHorizontal ? "H" : "V";

        string type = "CARRIER"; // TODO improve this
        string response = sendMessage("SHIP " + type + " " + to_string((int)origin.x) + " " + to_string((int)origin.y) +
                                      " " + h + "\n");

        if (!response.starts_with("OK"))
        {
            perror("Error on sending coordinates to server, exiting...\n");
            exit(1);
        }
    }
    for (int i = 0; i < NUMBER_OF_CRUISERS; i++)
    {
        Vector2 origin = fleet.cruisers[i].positions[0];
        string h = fleet.cruisers[i].isHorizontal ? "H" : "V";

        string type = "CRUISER"; // TODO improve this
        string response = sendMessage("SHIP " + type + " " + to_string((int)origin.x) + " " + to_string((int)origin.y) +
                                      " " + h + "\n");

        if (!response.starts_with("OK"))
        {
            perror("Error on sending coordinates to server, exiting...\n");
            exit(1);
        }
    }
    for (int i = 0; i < NUMBER_OF_BATTLESHIPS; i++)
    {
        Vector2 origin = fleet.battleships[i].positions[0];
        string h = fleet.battleships[i].isHorizontal ? "H" : "V";

        string type = "BATTLESHIP"; // TODO improve this
        string response = sendMessage("SHIP " + type + " " + to_string((int)origin.x) + " " + to_string((int)origin.y) +
                                      " " + h + "\n");

        if (!response.starts_with("OK"))
        {
            perror("Error on sending coordinates to server, exiting...\n");
            exit(1);
        }
    }
    for (int i = 0; i < NUMBER_OF_DESTROYERS; i++)
    {
        Vector2 origin = fleet.destroyers[i].positions[0];
        string h = fleet.destroyers[i].isHorizontal ? "H" : "V";

        string type = "DESTROYER"; // TODO improve this
        string response = sendMessage("SHIP " + type + " " + to_string((int)origin.x) + " " + to_string((int)origin.y) +
                                      " " + h + "\n");

        if (!response.starts_with("OK"))
        {
            perror("Error on sending coordinates to server, exiting...\n");
            exit(1);
        }
    }
    for (int i = 0; i < NUMBER_OF_SUBMARINES; i++)
    {
        Vector2 origin = fleet.submarines[i].positions[0];
        string h = fleet.submarines[i].isHorizontal ? "H" : "V";

        string type = "SUBMARINE"; // TODO improve this
        string response = sendMessage("SHIP " + type + " " + to_string((int)origin.x) + " " + to_string((int)origin.y) +
                                      " " + h + "\n");

        if (!response.starts_with("OK"))
        {
            perror("Error on sending coordinates to server, exiting...\n");
            exit(1);
        }
    }

    string response = sendMessage("READY\n");
    printf("received response after sending ready %s\n", response.c_str());

    if (!response.starts_with("START P1\n"))
    {
        perror("Error after READY...\n");
        exit(1);
    }

    getGame().gameState = WAITING_FOR_TURN;
}

void waitForTurn(void)
{
    while (true)
    {
        printf("waiting for turn\n");
        string turn = waitForResponse();
        vector<string> tokens = split(turn);

        string playerCode = tokens[1];
        printf("Turn of %s\n", playerCode.c_str());

        if (playerCode == getPlayer().code)
        {
            getGame().gameState = ATTACKING;
        }
    }
}

void connectToServer(string const &serverIp, int serverPort)
{
    printf("Connecting to server at %s:%d\n", serverIp.c_str(), serverPort);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server = {0};
    server.sin_family = AF_INET;
    server.sin_port = htons(serverPort);
    server.sin_addr.s_addr = inet_addr(serverIp.c_str());

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Invalid address");
        close(sock);
        sock = -1;
        exit(EXIT_FAILURE);
    }
}

void disconnectFromServer()
{
    if (sock != -1)
    {
        close(sock);
    }
}
