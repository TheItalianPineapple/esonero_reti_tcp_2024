/*
 ============================================================================
 Name        : server.c
 Author      : johnni_gallo
 Version     : 1.0
 Description : Esonero Reti 2024
 ============================================================================
 */


#include <stdio.h>
#include <time.h>
#include <winsock2.h>
#include <stdlib.h>
#include <string.h>
// Include Header
#include "protocol.h"

static void generate_password(char type, int length, char *password);
static void generate_numeric(char *password, int length);
static void generate_alpha(char *password, int length);
static void generate_mixed(char *password, int length);
static void generate_secure(char *password, int length);
static void handle_client(SOCKET client_socket);


int main() {
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    WSADATA wsaData;
    int client_len;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup Failed\n");
        return 1;
    }

    // Create Server Socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        printf("Socket Creation Failed %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    // Setup Server Address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(IP_SERVER);  // Use localhost IP (127.0.0.1) for local testing
    server_addr.sin_port = htons(PORT_SERVER);

    // Bind Socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind Failed %d\n", WSAGetLastError());
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // Listen For Incoming Connections
    // QLEN Makes Sure Only 5 Clients Can Go in Queue.
    if (listen(server_socket, QLEN) < 0	) {
        printf("Listen() failed");
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // Accept Incoming Connections
    printf("Server is listening for connections...\n");
    while (1) {
        client_len = sizeof(client_addr);
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_len);
        if (client_socket == INVALID_SOCKET) {
            printf("Accept Failed %d\n", WSAGetLastError());
            continue;
        }

        printf("New Connection From %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Handle The Client
        handle_client(client_socket);
    }

    // Close & Cleanup Socket
    closesocket(server_socket);
    WSACleanup();
    return 0;
}

static void generate_password(char type, int length, char *password) {
    switch (type) {
        case 'n':
            generate_numeric(password, length);
            break;
        case 'a':
            generate_alpha(password, length);
            break;
        case 'm':
            generate_mixed(password, length);
            break;
        case 's':
            generate_secure(password, length);
            break;
        default:
            strcpy(password, "ERROR: Invalid type (use n, a, m, or s)");
    }
}

// Function To Manage Client
void handle_client(SOCKET client_socket) {
    char input[BUFFER];
    char password[BUFFER];
    int bytes_received;

    // Loop
    while (1) {
        // Receive Client Request
        bytes_received = recv(client_socket, input, sizeof(input) - 1, 0);
        if (bytes_received <= 0) {
            printf("Error receiving data or client disconnected.\n");
            break;
        }
        input[bytes_received] = '\0';  // Null Terminate The String
        printf("Received request: %s\n", input);

        // Check If Client Decided To Quit
        if (strcmp(input, "q") == 0) {
            printf("Client Has Quit\n");
            break;
        }

        // Handle Client Input And Generate Password
        char type = input[0]; // Type
        int length = atoi(&input[2]); // Length
        if (length < 6 || length > 32) {
            printf("ERROR!, Password Length Must Be Between 6 And 32\n");  // ERROR If Size Does Not Follow Specific Restraints
        } else {
        	generate_password(type, length, password);
        }

        // Send The Output To Client
        send(client_socket, password, strlen(password), 0);
    }

    // Close Connection To Client
    closesocket(client_socket);
}



// Function To Generate A Numeric(123) Password Given A Defined Length
static void generate_numeric(char *password, int length) {
    const char digits[] = "1234567890";
    for (int i = 0; i < length; i++) {
        password[i] = digits[rand() % 10]; // Use rand() To Get Add A Random Char
    }
    password[length] = '\0';  // Null-terminate the string
}

// Function To Generate An Alphabetic(abc) Password Given A Defined Length
static void generate_alpha(char *password, int length) {
    const char alphabet[] = "abcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < length; i++) {
        password[i] = alphabet[rand() % 26]; // Use rand() To Get Add A Random Char
    }
    password[length] = '\0';  // Null-terminate the string
}

// Function To Generate A Mixed(abc, 123) Password Given A Defined Length
static void generate_mixed(char *password, int length) {
    const char alpha_numeric[] = "1234567890abcdefghijklmnopqrstuvwxyz";
    for (int i = 0; i < length; i++) {
        password[i] = alpha_numeric[rand() % 36]; // Use rand() To Get Add A Random Char
    }
    password[length] = '\0';  // Null-terminate the string
}

// Function To Generate A Secure(abc, 123, .,@) Password Given A Defined Lenght
static void generate_secure(char *password, int length) {
    const char secure_set[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789`~!@#$%^&*()-_=+[{]}|;:',<.>/?";
    int set_length = strlen(secure_set);
    for (int i = 0; i < length; i++) {
        password[i] = secure_set[rand() % set_length]; // Use rand() To Get Add A Random Char
    }
    password[length] = '\0';  // Null-terminate the string
}
