#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "temperature_sensor.h"
#include "optical_sensor.h"
#include "electrical_sensor.h"
#include "magnetic_sensor.h"
#include "acoustic_sensor.h"
#include "radiation_sensor.h"
#include "chemical_sensor.h"

#pragma comment(lib, "ws2_32.lib")

#define PORT_SENSOR 8080
#define PORT_CONTROL 8081
#define BUFFER_SIZE 1024

// Parameter codes
#define TEMPERATURE 1
#define RADIATION 2
#define CHEMICAL 3
#define OXYGEN 4
#define NOISE 5
#define VOLTAGE 6

// Threshold values for alerts
#define TEMP_THRESHOLD 40      // °C
#define RADIATION_THRESHOLD 20 // μSv/h
#define CHEMICAL_THRESHOLD 50  // ppm
#define OXYGEN_MIN_THRESHOLD 19 // % (below this is dangerous)
#define NOISE_THRESHOLD 85     // dB
#define VOLTAGE_THRESHOLD 500  // V/m

// Function to calculate electric field strength
double calculate_efield_strength(double voltage, double distance_m) {
    // Simple electric field calculation (V/m)
    return voltage / distance_m;
}

// Function to determine voltage safety level
int get_voltage_safety_level(double voltage) {
    if (voltage < 50.0) return 0;      // Safe
    if (voltage < 500.0) return 1;     // Low hazard
    if (voltage < 1000.0) return 2;    // Medium hazard
    return 3;                          // High hazard
}

void log_data(int param_code, int value) {
    time_t now;
    struct tm *local_time;
    char timestamp[26];
    char filename[20];
    FILE *fp;
    
    // Get current time
    time(&now);
    local_time = localtime(&now);
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", local_time);
    
    // Determine filename based on parameter code
    switch(param_code) {
        case TEMPERATURE:
            strcpy(filename, "temp.csv");
            break;
        case RADIATION:
            strcpy(filename, "radiation.csv");
            break;
        case CHEMICAL:
            strcpy(filename, "chemical.csv");
            break;
        case OXYGEN:
            strcpy(filename, "oxygen.csv");
            break;
        case NOISE:
            strcpy(filename, "noise.csv");
            break;
        case VOLTAGE:
            strcpy(filename, "voltage.csv");
            break;
        default:
            strcpy(filename, "unknown.csv");
    }
    
    // Check if file exists to write headers
    fp = fopen(filename, "r");
    if (fp == NULL) {
        fp = fopen(filename, "w");
        fprintf(fp, "Timestamp,Value\n");
    } else {
        fclose(fp);
        fp = fopen(filename, "a");
    }
    
    // Log data
    fprintf(fp, "%s,%d\n", timestamp, value);
    fclose(fp);
    
    printf("Logged %d to %s at %s\n", value, filename, timestamp);
}

void send_alert_to_control(int param_code, int value) {
    SOCKET sock = INVALID_SOCKET;
    struct sockaddr_in serv_addr;
    
    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation error: %d\n", WSAGetLastError());
        return;
    }
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT_CONTROL);
    
    // Convert IPv4 address from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("Invalid address/ Address not supported\n");
        closesocket(sock);
        return;
    }
    
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("Connection Failed to Control Module: %d\n", WSAGetLastError());
        closesocket(sock);
        return;
    }
    
    // Send data as integers
    int data[2] = {param_code, value};
    send(sock, (char*)data, sizeof(data), 0);
    printf("Alert sent to control: Parameter Code %d, Value %d\n", param_code, value);
    
    closesocket(sock);
}

void check_threshold(int param_code, int value) {
    int alert = 0;
    
    switch(param_code) {
        case TEMPERATURE:
            if (value > TEMP_THRESHOLD) alert = 1;
            break;
        case RADIATION:
            if (value > RADIATION_THRESHOLD) alert = 1;
            break;
        case CHEMICAL:
            if (value > CHEMICAL_THRESHOLD) alert = 1;
            break;
        case OXYGEN:
            if (value < OXYGEN_MIN_THRESHOLD) alert = 1;
            break;
        case NOISE:
            if (value > NOISE_THRESHOLD) alert = 1;
            break;
        case VOLTAGE:
            if (value > VOLTAGE_THRESHOLD) alert = 1;
            break;
    }
    
    if (alert) {
        printf("ALERT: Parameter %d exceeded threshold with value %d\n", param_code, value);
        send_alert_to_control(param_code, value);
    }
}

// Process sensor readings with appropriate sensor models
// Process sensor readings with appropriate sensor models
double process_sensor_reading(int param_code, int raw_value) {
    double processed_value = raw_value;
    
    switch(param_code) {
        case TEMPERATURE: {
            // Use RTD sensor model for temperature
            double rtd_reading = read_rtd_temperature((double)raw_value, 2); // Assuming 2 years in service
            double resistance = calculate_rtd_resistance((double)raw_value);
            printf("RTD Sensor reading: %.2f°C (raw: %d°C)\n", rtd_reading, raw_value);
            printf("RTD Resistance: %.2f ohms\n", resistance);
            processed_value = rtd_reading;
            break;
        }
        case RADIATION: {
            // Simulate radiation counts for the given level
            int counts = simulate_radiation_counts((double)raw_value, 10.0); // 10-second integration
            int detection = detect_radiation((double)raw_value);
            printf("Radiation detector: %d counts in 10s, Detection: %s\n", 
                   counts, detection ? "POSITIVE" : "NEGATIVE");
            printf("Radiation sensitivity: %.1f counts per μSv\n", RAD_SENSITIVITY);
            
            // Create a simple spectrum for demonstration
            int spectrum[128] = {0};
            for (int i = 0; i < 128; i++) {
                spectrum[i] = (int)(counts * exp(-(pow(i - raw_value/2, 2) / 200.0)));
            }
            
            int isotope = identify_isotope(spectrum, 128);
            if (isotope > 0) {
                printf("Isotope identification: Type %d detected\n", isotope);
            }
            
            processed_value = raw_value;
            break;
        }
        case CHEMICAL: {
            // Simulate electrochemical sensor for CO gas
            double interfering_conc[7] = {0}; // No interfering gases in this simulation
            double sensor_current = ec_sensor_current(GAS_CO, (double)raw_value, interfering_conc);
            double corrected_conc = current_to_concentration(sensor_current, GAS_CO);
            
            // Apply temperature effect (assuming 25°C)
            corrected_conc = apply_temperature_effect(corrected_conc, 25.0);
            
            printf("Chemical sensor: %.2f ppm CO (raw: %d ppm)\n", corrected_conc, raw_value);
            printf("Sensor current: %.2f nA, Zero current: %.2f nA\n", sensor_current, EC_ZERO_CURRENT);
            printf("Sensitivity: %.2f nA/ppm\n", EC_SENSITIVITY);
            processed_value = corrected_conc;
            break;
        }
        case NOISE: {
            // Convert dB to Pascal for acoustic sensor
            double pascal_value = dbspl_to_pascal((double)raw_value);
            double mic_voltage = mic_output_voltage(pascal_value);
            
            // Apply frequency response (assuming 1kHz noise)
            double freq_adjusted = apply_frequency_response(mic_voltage, 1000.0);
            
            printf("Acoustic sensor: %.2f dB SPL (raw: %d dB), Mic output: %.6f V\n", 
                   raw_value, raw_value, freq_adjusted);
            processed_value = raw_value;
            break;
        }
        case VOLTAGE: {
            // Use Hall effect sensor for voltage/field measurement
            double field_strength = calculate_efield_strength((double)raw_value, 1.0); // Assuming 1m distance
            int safety_level = get_voltage_safety_level((double)raw_value);
            
            printf("Voltage sensor: %d V\n", raw_value);
            printf("Electric field strength: %.2f V/m\n", field_strength);
            printf("Safety level: %d\n", safety_level);
            
            // Simulate Hall effect sensor output
            double magnetic_field = raw_value / 100.0; // Simplified conversion
            double hall_output = hall_effect_output(magnetic_field);
            printf("Hall sensor output: %.3f V (for %.2f mT)\n", hall_output, magnetic_field);
            
            // Simulate proximity detection
            int voltage_detected = detect_voltage_presence((double)raw_value, 0.5); // 0.5m distance
            if (voltage_detected) {
                printf("WARNING: Live voltage detected in proximity!\n");
            }
            
            processed_value = raw_value;
            break;
        }
    }
    
    return processed_value;
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
    address.sin_port = htons(PORT_SENSOR);
    
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
    
    printf("Sensor module started. Listening on port %d...\n", PORT_SENSOR);
    printf("Date: March 01, 2025, 10:30 PM IST\n");
    printf("Smart Suit for Industrial Workers - Sensor Module\n");
    printf("------------------------------------------------\n");
    
    // Initialize random seed for sensor simulation
    srand((unsigned int)time(NULL));
    
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
            
            printf("\nReceived from environment: Parameter Code %d (%s), Value %d\n", 
                   param_code, get_param_name(param_code), value);
            
            // Process sensor reading with appropriate sensor model
            double processed_value = process_sensor_reading(param_code, value);
            
            // Log data to CSV (using the original value for consistency)
            log_data(param_code, value);
            
            // Check if value exceeds threshold
            check_threshold(param_code, value);
        }
        
        closesocket(new_socket);
    }
    
    closesocket(server_fd);
    WSACleanup();
    return 0;
}
