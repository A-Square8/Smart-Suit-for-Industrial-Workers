#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT_ACTUATOR 8082
#define BUFFER_SIZE 1024

// Actuator response codes
#define COOLING_ON 101
#define HEATING_ON 102
#define RADIATION_ALARM 201
#define CHEMICAL_ALARM 301
#define OXYGEN_ALARM 401
#define NOISE_PROTECTION 501
#define VOLTAGE_WARNING 601

// Acknowledgment codes
#define ACK_SUCCESS 1
#define ACK_FAILURE 0

void activate_actuator(int response_code, int value) {
    // This function would control the physical actuators in a real system
    // For simulation, we just print messages
    
    time_t now;
    struct tm *local_time;
    char timestamp[26];
    
    // Get current time
    time(&now);
    local_time = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", local_time);
    
    printf("\n[%s] ACTUATOR ACTIVATED: ", timestamp);
    
    switch(response_code) {
        case COOLING_ON:
            printf("Cooling system activated. Temperature: %d°C\n", value);
            printf("Action: Activating cooling elements in suit to reduce temperature.\n");
            break;
        case HEATING_ON:
            printf("Heating system activated. Temperature: %d°C\n", value);
            printf("Action: Activating heating elements in suit to increase temperature.\n");
            break;
        case RADIATION_ALARM:
            printf("RADIATION ALERT! Level: %d μSv/h\n", value);
            printf("Action: Activating radiation shield and haptic warning system.\n");
            printf("Warning: Evacuate area immediately!\n");
            break;
        case CHEMICAL_ALARM:
            printf("CHEMICAL HAZARD ALERT! Concentration: %d ppm\n", value);
            printf("Action: Sealing suit interfaces and activating filtration system.\n");
            printf("Warning: Hazardous chemical detected, evacuate area!\n");
            break;
        case OXYGEN_ALARM:
            printf("LOW OXYGEN ALERT! Level: %d%%\n", value);
            printf("Action: Activating emergency oxygen supply.\n");
            printf("Warning: Oxygen levels dangerously low, evacuate area!\n");
            break;
        case NOISE_PROTECTION:
            printf("HIGH NOISE LEVEL! %d dB detected\n", value);
            printf("Action: Activating acoustic dampening system.\n");
            printf("Warning: Prolonged exposure may cause hearing damage.\n");
            break;
        case VOLTAGE_WARNING:
            printf("HIGH VOLTAGE FIELD! %d V/m detected\n", value);
            printf("Action: Activating electrical insulation layer.\n");
            printf("Warning: High voltage field detected, maintain safe distance from sources.\n");
            break;
        default:
            printf("Unknown response code: %d\n", response_code);
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
    address.sin_port = htons(PORT_ACTUATOR);
    
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
    
    printf("Actuator module started. Listening on port %d...\n", PORT_ACTUATOR);
    
    while (1) {
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen)) == INVALID_SOCKET) {
            printf("Accept error: %d\n", WSAGetLastError());
            continue;
        }
        
        int data[2];
        int valread = recv(new_socket, (char*)data, sizeof(data), 0);
        
        if (valread > 0) {
            int response_code = data[0];
            int value = data[1];
            
            printf("Received from control: Response Code %d (%s), Value %d\n", 
                   response_code, get_response_name(response_code), value);
            
            // Activate the appropriate actuator
            activate_actuator(response_code, value);
            
            // Send acknowledgment back to control
            int ack = ACK_SUCCESS;
            send(new_socket, (char*)&ack, sizeof(ack), 0);
            printf("Acknowledgment sent to control: %d\n", ack);
        }
        
        closesocket(new_socket);
    }
    
    closesocket(server_fd);
    WSACleanup();
    return 0;
}
