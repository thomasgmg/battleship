#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <stdio.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

int main()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address = {0};
    address.sin_family = AF_INET;
    address.sin_port = htons(12345);
    address.sin_addr.s_addr = INADDR_ANY;

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 1);
    int client_fd = accept(server_fd, nullptr, nullptr);

    char buffer[64] = {0};
    string input;

    while (true)
    {
        // Receive client’s move
        int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0)
        {
            buffer[bytes_received] = '\0';
            printf("Received: %s\n", buffer);
            if (strcmp(buffer, "q") == 0)
            { // Check if client sent "q"
                printf("Client requested quit\n");
                break;
            }
        }
        else if (bytes_received == 0)
        {
            printf("Client disconnected\n");
            break;
        }

        // Send response to client
        cout << "Enter response to client (or 'q' to exit): " << endl;
        getline(cin, input);
        if (input == "q" || input == "quit")
        {
            send(client_fd, "q", 1, 0); // Tell client to quit
            break;
        }
        send(client_fd, input.c_str(), input.length(), 0);
        // printf("Sent: %s\n", input.c_str());
    }

    close(client_fd);
    close(server_fd);
    return 0;
}
