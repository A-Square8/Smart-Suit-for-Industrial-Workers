#ifndef OPTICAL_SENSOR_H
#define OPTICAL_SENSOR_H

#include <stdlib.h>
#include <math.h>

// Photodiode constants
#define PHOTO_MIN_LUX 0.001
#define PHOTO_MAX_LUX 100000.0
#define PHOTO_SENSITIVITY 0.5  // A/W
#define PHOTO_DARK_CURRENT 2.0  // nA
#define PHOTO_RESPONSE_TIME 10  // Response time in microseconds

// IR proximity constants
#define PROX_MIN_DISTANCE 0.05  // Minimum distance in meters
#define PROX_MAX_DISTANCE 1.5  // Maximum distance in meters
#define PROX_ACCURACY 0.05  // Accuracy as fraction of distance

// Function to simulate photodiode output current
double photodiode_current(double light_intensity_lux) {
    // Convert lux to W/m²
    double irradiance = light_intensity_lux / 683.0;  // Approximate conversion
    
    // Calculate photocurrent (simplified)
    double photocurrent = PHOTO_SENSITIVITY * irradiance * 1e-6;  // in Amperes
    
    // Add dark current and noise
    double dark_current = PHOTO_DARK_CURRENT * 1e-9;  // Convert nA to A
    double noise = ((rand() % 201) - 100) / 1000.0 * dark_current;
    
    return photocurrent + dark_current + noise;
}

// Function to simulate IR proximity sensor reading
double read_proximity(double actual_distance) {
    if (actual_distance < PROX_MIN_DISTANCE) {
        return PROX_MIN_DISTANCE;
    }
    if (actual_distance > PROX_MAX_DISTANCE) {
        return PROX_MAX_DISTANCE;
    }
    
    // Add measurement noise
    double noise = actual_distance * ((rand() % 201) - 100) / 1000.0 * PROX_ACCURACY;
    return actual_distance + noise;
}

#endif
