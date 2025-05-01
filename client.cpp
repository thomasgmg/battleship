#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <netinet/in.h>
#include <stdio.h>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
#include <stdio.h>

#include "client.h"
#include "fleet.h"
#include "game.h"
#include "utils.h"

using namespace std;

int sock = -1;

// func declarations

void attackLoop();

// -------------------------

string waitForResponse(void)
{
    const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE] = {0};

    // Receive server response
    int bytes_received = recv(sock, buffer, BUFFER_SIZE - 1, 0);
    if(bytes_received == 0) 
    {
        printf("server closed socket, cleaning up on our side...\n");
        close(sock);
        return string();
    }
    if (bytes_received < 0)
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

    if (getPlayer().code == "P1")
    {
        getGame().gameState = ATTACKING;
    }
    else
    {
        getGame().gameState = WAITING_FOR_TURN;
    }

    thread t(attackLoop);
    t.detach();
}

void attackLoop(void)
{
    // TODO temporary
    static int x = 0;
    static int y = 0;

    while (true)
    {
        string message = waitForResponse();
        printf("client received message: %s\n", message.c_str());

        if(message.empty())
        {
            printf("server disconnected, exiting attack loop\n");
            break;
        }

        vector<string> tokens = split(message);
        if (tokens[0] == "TURN" && tokens[1] == getPlayer().code)
        {
            getGame().gameState = ATTACKING;

            for (int i = 0; i != 3; ++i) 
            {
                char attack_ctr[32];
                
                message = sendMessage(string(attack_ctr , sprintf(attack_ctr, "ATTACK %d %d", x, y++)));
                printf("attack result: %s", message.c_str());

                if (tokens[0] == "WIN")
                {
                    printf("Game over\n");
                    getGame().gameState = GAME_OVER;
                    continue; // wait for close socket "response"
                }
                if (y == 10)
                {
                    y = 0;
                    ++x;
                }
                if(x == 10) 
                {
                    printf("this shouldn't happen, exiting loop\n");
                    return;
                }
            }

            getGame().gameState = WAITING_FOR_TURN;
        }
        else if (tokens[0] == "TURN")
        {
            printf("waiting for turn\n");
            getGame().gameState = WAITING_FOR_TURN;
            // TODO save the shot somewhere and show in the GUI
        }

        else if (tokens[0] == "WIN") {
            printf("Game over\n");
            getGame().gameState = GAME_OVER;
            continue; // wait for close socket "response"
            
        }
    }

    printf("exiting attack loop thread\n");
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
