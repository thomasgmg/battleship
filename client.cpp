#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <stdio.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

#include "client.h"
#include "fleet.h"

using namespace std;

int sock = 0;

void sendMessage(const string &message)
{
    send(sock, message.c_str(), message.size(), 0);
}

void sendFleet(Fleet const &fleet)
{
}

void loop(void)
{
    sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server = {0};
    server.sin_family = AF_INET;
    server.sin_port = htons(12345);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Invalid address");
        close(sock);
        exit(EXIT_FAILURE);
    }

    const int BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE] = {0}; // Buffer for sending/receiving messages

    while (true)
    {
        // Receive server response
        int bytes_received = recv(sock, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0)
        {
            if (bytes_received == 0)
            {
                printf("Server disconnected\n");
            }
            else
            {
                perror("Receive failed");
            }
            break;
        }

        // Null-terminate the received message
        buffer[bytes_received] = '\0';
        printf("Server response: %s\n", buffer);
    }

    // TODO while should check for termination in the condition
    close(sock);
}
