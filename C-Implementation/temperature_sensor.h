#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H

#include <math.h>
#include <stdlib.h>

// RTD constants for PT100 sensor (industrial standard)
#define RO 100.0  // Resistance at 0°C
#define ALPHA 0.00385  // Temperature coefficient

// Accuracy and range parameters
#define TEMP_MIN -200.0
#define TEMP_MAX 850.0
#define TEMP_ACCURACY 0.3  // Accuracy in °C
#define TEMP_RESPONSE_TIME 500  // Response time in milliseconds

// Drift parameters
#define DRIFT_RATE 0.005  // 0.5% per year
#define SENSOR_ERROR 0.1  // Base error in °C

// Function to calculate RTD resistance at given temperature
double calculate_rtd_resistance(double temperature) {
    return RO * (1 + ALPHA * temperature);
}

// Function to simulate RTD sensor reading with noise and drift
double read_rtd_temperature(double actual_temp, int years_in_service) {
    double ideal_resistance = calculate_rtd_resistance(actual_temp);
    double drift_factor = 1 + (DRIFT_RATE * years_in_service);
    double noise = ((rand() % 201) - 100) / 1000.0 * SENSOR_ERROR;
    double measured_resistance = ideal_resistance * drift_factor + noise;
    
    // Convert back to temperature
    return (measured_resistance / RO - 1) / ALPHA;
}

#endif
