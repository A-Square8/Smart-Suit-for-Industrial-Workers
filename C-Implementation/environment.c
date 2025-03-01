#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT_SENSOR 8080
#define BUFFER_SIZE 1024

// Parameter codes
#define TEMPERATURE 1
#define RADIATION 2
#define CHEMICAL 3
#define OXYGEN 4
#define NOISE 5
#define VOLTAGE 6

void send_to_sensor(int param_code, int value) {
    SOCKET sock = INVALID_SOCKET;
    struct sockaddr_in serv_addr;
    
    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation error: %d\n", WSAGetLastError());
        return;
    }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT_SENSOR);
    
    // Convert IPv4 address from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Invalid address/ Address not supported\n");
        closesocket(sock);
        return;
    }
    
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection Failed: %d\n", WSAGetLastError());
        closesocket(sock);
        return;
    }
    
    // Send data as integers
    int data[2] = {param_code, value};
    send(sock, (char*)data, sizeof(data), 0);
    printf("Sent to sensor: Parameter Code %d, Value %d\n", param_code, value);
    
    closesocket(sock);
}

void display_menu() {
    printf("\n===== Environment Simulation Menu =====\n");
    printf("1. Change Temperature (°C)\n");
    printf("2. Change Radiation Level (μSv/h)\n");
    printf("3. Change Chemical Concentration (ppm)\n");
    printf("4. Change Oxygen Level (%%)\n");
    printf("5. Change Noise Level (dB)\n");
    printf("6. Change Electrical Field (V/m)\n");
    printf("0. Exit\n");
    printf("Enter your choice: ");
}

const char* get_param_name(int code) {
    switch(code) {
        case TEMPERATURE: return "Temperature";
        case RADIATION: return "Radiation";
        case CHEMICAL: return "Chemical";
        case OXYGEN: return "Oxygen";
        case NOISE: return "Noise";
        case VOLTAGE: return "Voltage";
        default: return "Unknown";
    }
}

int main() {
    WSADATA wsaData;
    int choice, value;
    
    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }
    
    printf("Smart Suit for Industrial Workers - Environment Simulation\n");
    printf("--------------------------------------------------------\n");
    
    while (1) {
        display_menu();
        scanf("%d", &choice);
        
        if (choice == 0) {
            printf("Exiting environment simulation...\n");
            break;
        }
        
        if (choice >= 1 && choice <= 6) {
            printf("Enter new %s value: ", get_param_name(choice));
            scanf("%d", &value);
            send_to_sensor(choice, value);
        } else {
            printf("Invalid choice. Please try again.\n");
        }
    }
    
    // Cleanup Winsock
    WSACleanup();
    return 0;
}
