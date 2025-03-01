#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT_CONTROL 8081
#define PORT_ACTUATOR 8082
#define BUFFER_SIZE 1024

// Parameter codes
#define TEMPERATURE 1
#define RADIATION 2
#define CHEMICAL 3
#define OXYGEN 4
#define NOISE 5
#define VOLTAGE 6

// Actuator response codes
#define COOLING_ON 101
#define HEATING_ON 102
#define RADIATION_ALARM 201
#define CHEMICAL_ALARM 301
#define OXYGEN_ALARM 401
#define NOISE_PROTECTION 501
#define VOLTAGE_WARNING 601

void send_to_actuator(int response_code, int value) {
    SOCKET sock = INVALID_SOCKET;
    struct sockaddr_in serv_addr;
    
    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation error: %d\n", WSAGetLastError());
        return;
    }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT_ACTUATOR);
    
    // Convert IPv4 address from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Invalid address/ Address not supported\n");
        closesocket(sock);
        return;
    }
    
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection Failed to Actuator Module: %d\n", WSAGetLastError());
        closesocket(sock);
        return;
    }
    
    // Send data as integers
    int data[2] = {response_code, value};
    send(sock, (char*)data, sizeof(data), 0);
    printf("Command sent to actuator: Response Code %d, Value %d\n", response_code, value);
    
    // Wait for acknowledgment
    int ack;
    recv(sock, (char*)&ack, sizeof(ack), 0);
    printf("Received acknowledgment from actuator: %d\n", ack);
    
    closesocket(sock);
}

int determine_response(int param_code, int value) {
    switch(param_code) {
        case TEMPERATURE:
            return (value > 30) ? COOLING_ON : HEATING_ON;
        case RADIATION:
            return RADIATION_ALARM;
        case CHEMICAL:
            return CHEMICAL_ALARM;
        case OXYGEN:
            return OXYGEN_ALARM;
        case NOISE:
            return NOISE_PROTECTION;
        case VOLTAGE:
            return VOLTAGE_WARNING;
        default:
            return 0;
    }
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

const char* get_response_name(int code) {
    switch(code) {
        case COOLING_ON: return "Cooling System Activation";
        case HEATING_ON: return "Heating System Activation";
        case RADIATION_ALARM: return "Radiation Alarm";
        case CHEMICAL_ALARM: return "Chemical Hazard Alarm";
        case OXYGEN_ALARM: return "Oxygen Level Alarm";
        case NOISE_PROTECTION: return "Noise Protection";
        case VOLTAGE_WARNING: return "Electrical Field Warning";
        default: return "Unknown Response";
    }
}

int main() {
    WSADATA wsaData;
    SOCKET server_fd = INVALID_SOCKET, new_socket = INVALID_SOCKET;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed: %d\n", WSAGetLastError());
        return 1;
    }
    
    // Creating socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation error: %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }
    
    // Setup socket options to allow address reuse
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) == SOCKET_ERROR) {
        printf("Setsockopt error: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT_CONTROL);
    
    // Bind the socket to the port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
        printf("Bind failed: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }
    
    if (listen(server_fd, 3) == SOCKET_ERROR) {
        printf("Listen error: %d\n", WSAGetLastError());
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }
    
    printf("Control module started. Listening on port %d...\n", PORT_CONTROL);
    
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) == INVALID_SOCKET) {
            printf("Accept error: %d\n", WSAGetLastError());
            continue;
        }
        
        int data[2];
        int valread = recv(new_socket, (char*)data, sizeof(data), 0);
        
        if (valread > 0) {
            int param_code = data[0];
            int value = data[1];
            
            printf("Received alert from sensor: Parameter Code %d (%s), Value %d\n", 
                   param_code, get_param_name(param_code), value);
            
            // Determine appropriate response
            int response_code = determine_response(param_code, value);
            
            if (response_code > 0) {
                printf("Determined response: %d (%s)\n", 
                       response_code, get_response_name(response_code));
                
                // Send command to actuator
                send_to_actuator(response_code, value);
            }
        }
        
        closesocket(new_socket);
    }
    
    closesocket(server_fd);
    WSACleanup();
    return 0;
}
