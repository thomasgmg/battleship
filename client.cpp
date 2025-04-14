#include <arpa/inet.h>
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
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server = {0};
    server.sin_family = AF_INET;
    server.sin_port = htons(12345);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(sock, (struct sockaddr *)&server, sizeof(server));

    char buffer[64] = {0};
    string input;

    while (true)
    {
        // Send move to server
        cout << "Enter move to server (or 'q' to exit): " << endl;
        getline(cin, input);
        if (input == "q" || input == "quit")
        {
            send(sock, "q", 1, 0); // Send "q" to server
            break;
        }
        send(sock, input.c_str(), input.length(), 0);
        // printf("Sent: %s\n", input.c_str());

        // Receive server’s response
        int bytes_received = recv(sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0)
        {
            buffer[bytes_received] = '\0';
            printf("Received: %s\n", buffer);
            if (strcmp(buffer, "q") == 0)
            { // Check if server sent "q"
                printf("Server requested quit\n");
                break;
            }
        }
        else if (bytes_received == 0)
        {
            printf("Server disconnected\n");
            break;
        }
    }

    close(sock);
    return 0;
}
