#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>
#include<winsock2.h>
#include <stdbool.h>

#pragma comment(lib,"ws2_32.lib")


#define PORT 80
#define REPLY_BUFFER 32767
#define SEND_BUFFER 512
#define ADDRESS_ENTRY_BUFFER 32
#define DEFAULT_SV_ADDRESS "142.250.105.100"
#define NET_MSG_CAP " / HTTP / 1.1\r\n\r\n"



bool error_handler(const char* error_msg, int error_code)
{
    printf("[ERROR]: %s. Code: %i.\n", error_msg, error_code);
    return false;
}


bool initialize_wsa(WSADATA* wsa)
{
    printf("\nInitializing WSA...\n");

    int wsa_result = WSAStartup(MAKEWORD(2, 2), wsa);
    if (wsa_result != 0)
    {
        return error_handler("Failed to startup WSA", wsa_result);
    }

    printf("[SUCCESS]: WSA initialization complete.\n");

    return true;
}

bool initialize_socket(SOCKET* w_socket)
{
    printf("Initializing socket...\n");

    *w_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (*w_socket == INVALID_SOCKET)
    {
        return error_handler("Socket initialization error", INVALID_SOCKET);
    }

    printf("[SUCCESS]: Socket initialization complete.\n\n");
    return true;
}


bool connect_to_server(SOCKET w_socket, sockaddr_in server)
{
    printf("Attempting to make connection...\n");

    if (connect(w_socket, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
    {
        return error_handler("Failed to connect", SOCKET_ERROR);
    }
    printf("[SUCCESS]: Connected to address.\n");

    return true;
}

bool send_data(SOCKET w_socket, char net_msg[SEND_BUFFER])
{
    printf("\nAttempting to send data...\n");

    size_t send_result = send(w_socket, net_msg, strlen(net_msg), 0);
    if (send(w_socket, net_msg, strlen(net_msg), 0) == SOCKET_ERROR)
    {
        return error_handler("Failed to send data", SOCKET_ERROR);
    }
    printf("[SUCCESS]: Data sent.  Size: %zu bytes.\n", send_result);

    return true;
}

bool receive_data(SOCKET w_socket, char server_reply[REPLY_BUFFER])
{
    printf("Attempting to receive data...\n");

    size_t receive_result = recv(w_socket, server_reply, REPLY_BUFFER - 4, 0);
    if (receive_result == SOCKET_ERROR)
    {
        return error_handler("Failed to receive data", SOCKET_ERROR);
    }

    printf("[SUCCESS]: Data received.  Size: %zu bytes.\n", receive_result);

    server_reply[receive_result] = '\0';
    printf("\n========DATA START========\n\n%s\n\n========DATA END========\n", server_reply);

    return true;
}


bool config_connection(sockaddr_in* server, const char* address, int address_family, int port)
{
    printf("Attempting to configure connection...\n");
    if (address == NULL)
    {
        printf("Invalid IP entry. Restoring to default\n");
        address = DEFAULT_SV_ADDRESS;
    }

    server->sin_addr.s_addr = inet_addr(address);
    server->sin_family = address_family;
    server->sin_port = htons(port);


    printf("[SUCCESS]: Configuration complete. Connecting to %s:%i.\n", address, port);
    return true;
}

void end_connection(SOCKET w_socket)
{
    closesocket(w_socket);
    WSACleanup();
}




int main(int argc, char* argv[])
{
    const float version = 0.9;
    printf("===HTTP Reqstr===\n--Version: %.1f--\n--Made by: Rim032--\n\n", version);

    char desired_address[ADDRESS_ENTRY_BUFFER];
    char net_msg[SEND_BUFFER];

    printf("Desired IP address: ");
    if (fgets(desired_address, sizeof(desired_address), stdin) == NULL)
    {
        return error_handler("Bad address input", NULL);
    }

    printf("Desired HTTP request(s): ");
    if (fgets(net_msg, sizeof(net_msg), stdin) == NULL)
    {
        return error_handler("Bad net message input", NULL);
    }

    char* net_msg_ptr = (char*)malloc(sizeof(net_msg));
    char* desired_address_ptr = (char*)malloc(sizeof(net_msg));


    WSADATA wsa;
    SOCKET w_socket;

    initialize_wsa(&wsa);
    initialize_socket(&w_socket);
    strcat(net_msg, NET_MSG_CAP);

    struct sockaddr_in server;

    char server_reply[REPLY_BUFFER];
    char* server_reply_ptr = (char*)malloc(REPLY_BUFFER);

    WSADATA* wsa_ptr = (WSADATA*)malloc(sizeof(wsa));
    SOCKET* w_socket_ptr = (SOCKET*)malloc(sizeof(w_socket));
    struct sockaddr_in* server_ptr = (struct sockaddr_in*)malloc(sizeof(server));


    config_connection(&server, desired_address, AF_INET, PORT);

    connect_to_server(w_socket, server);
    send_data(w_socket, net_msg);
    receive_data(w_socket, server_reply);

    end_connection(w_socket);



    free(net_msg_ptr);
    free(desired_address_ptr);
    free(server_reply_ptr);

    free(wsa_ptr);
    free(w_socket_ptr);
    free(server_ptr);

    printf("\nPress enter to exit...");
    scanf("%c", stdin);

    return 0;
}