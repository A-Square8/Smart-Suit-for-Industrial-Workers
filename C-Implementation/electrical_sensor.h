#ifndef ELECTRICAL_SENSOR_H
#define ELECTRICAL_SENSOR_H

#include <stdlib.h>
#include <math.h>

// Hall effect sensor constants
#define HALL_SENSITIVITY 5.0  // mV/mT
#define HALL_OFFSET 2.5  // V
#define HALL_MAX_FIELD 2000.0  // mT
#define HALL_RESPONSE_TIME 3  // Response time in microseconds

// Non-contact voltage detector constants
#define NCVD_MIN_VOLTAGE 50.0  // Minimum detectable voltage in V
#define NCVD_MAX_DISTANCE 0.1  // Maximum detection distance in meters
#define NCVD_THRESHOLD 1.0  // Detection threshold in V

// Function to simulate Hall effect sensor output
double hall_effect_output(double magnetic_field_mT) {
    if (magnetic_field_mT > HALL_MAX_FIELD) {
        magnetic_field_mT = HALL_MAX_FIELD;
    }
    
    // Calculate output voltage
    double output = HALL_OFFSET + (HALL_SENSITIVITY * magnetic_field_mT / 1000.0);
    
    // Add noise
    double noise = ((rand() % 201) - 100) / 10000.0;  // ±0.01V noise
    
    return output + noise;
}

// Function to simulate current measurement using Hall effect
double measure_current_hall(double actual_current) {
    // Convert current to magnetic field (simplified model)
    double magnetic_field = actual_current * 5.0;  // Assuming 5 mT per Amp
    
    // Get Hall sensor output
    double hall_voltage = hall_effect_output(magnetic_field);
    
    // Convert back to current
    double measured_current = (hall_voltage - HALL_OFFSET) * 1000.0 / (HALL_SENSITIVITY * 5.0);
    
    return measured_current;
}

// Function to detect AC voltage presence
int detect_voltage_presence(double voltage, double distance) {
    if (voltage < NCVD_MIN_VOLTAGE || distance > NCVD_MAX_DISTANCE) {
        return 0;  // No detection
    }
    
    // Calculate detection strength (simplified inverse square law)
    double strength = voltage * pow(NCVD_MAX_DISTANCE / distance, 2);
    
    return (strength > NCVD_THRESHOLD) ? 1 : 0;
}

#endif
