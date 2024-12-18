/*
 ============================================================================
 Name        : client.c
 Author      : johnni_gallo
 Version     : 1.0
 Description : Esonero Reti 2024
 ============================================================================
 */

#include <stdio.h>
#include <winsock2.h>
#include <stdlib.h>
#include <string.h>

// Include Header
#include "protocol.h"

static int handle_user_input(SOCKET server_socket);
static void print_menu(void);

int main() {
    WSADATA wsaData;
    SOCKET client_socket;
    struct sockaddr_in server_addr;

    // Initializing Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }

    // Create Client Socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        printf("Socket creation failed: %d\n", WSAGetLastError());
        WSACleanup(); // Clean Up If ERROR in Creating Socket
        return 1;
    }

    // Server Address Setup
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(IP_SERVER);  // Use (127.0.0.1) To Test In Local Enviroment
    server_addr.sin_port = htons(PORT_SERVER);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Connection failed: %d\n", WSAGetLastError());
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    printf("Connected to the server.\n");

    // Keep Server-Client in a loop
    while (1) {
        if (handle_user_input(client_socket) == 1) {
            break;  // Break The Loop If Client Quits (q)
        }
    }

    // Close & Cleanup Socket
    closesocket(client_socket);
    WSACleanup();
    return 0;
}


// Function To Manage Input
static int handle_user_input(SOCKET server_socket) {
    char input[BUFFER];  // Input Var
    char password[BUFFER]; // Password Buffer
    int bytes_rec;

    // Explanation Of Server's Use
    print_menu();
    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = 0;  // Remove newline character



    // If User Types 'q', Quit
    if (strcmp(input, "q") == 0) {
        send(server_socket, "q", 1, 0);  // Let Server Know Client Is Quitting
        printf("Goodbye!\n");
        return 1;
    }

    // Send The Input To Server
    if (send(server_socket, input, strlen(input), 0) == SOCKET_ERROR) {
        printf("Error sending data to the server\n");
        return 0; // Return 0 If Error Is Caught
    }


    // Receive Data From Server
    bytes_rec = recv(server_socket, password, sizeof(password) - 1, 0);
    if (bytes_rec<=0) {
        printf("Error receiving answer from server\n");
        return 0; //Return 0 If Error Is Caught
    }
    password[bytes_rec] = '\0'; // Null Terminate The String

    printf("Generated password: %s\n", password);
    return 0; // Continue The Loop
}

static void print_menu(void) {
    printf("\n=== Password Generator ===\n");
    printf("Enter type and length (e.g., 'n 10' for 10-digit numeric password)\n");
    printf("Types available:\n");
    printf("  n - Numeric (0-9)\n");
    printf("  a - Alphabetic (a-z)\n");
    printf("  m - Mixed (alphanumeric)\n");
    printf("  s - Secure (includes special characters)\n");
    printf("  q - Quit\n");
    printf("Length must be between 6-32\n");
    printf(">>> ");
}
